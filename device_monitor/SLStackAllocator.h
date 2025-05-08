#pragma once

#include <atomic>
#include <stdint.h>
#include <optional>
#include <thread>

template <size_t SZ, uint8_t ALIGN>
class SLStackAllocator
{
public:
	SLStackAllocator();
	~SLStackAllocator();

	SLStackAllocator(const SLStackAllocator& rhs) = delete;
	SLStackAllocator& operator=(const SLStackAllocator& rhs) = delete;
	SLStackAllocator & operator=(const SLStackAllocator && rhs) = delete;
	
	void* alloc(size_t size);
	void free(void* memory);

private:

	struct Header
	{
		std::atomic<uint8_t> locked;
		uint32_t size;
	};

	struct Footer
	{
		uint32_t blockStart;
	};

private:
	std::optional<uint32_t> ReserveBlock(size_t sz);
	uint32_t InitBlock(uint32_t blockStart, size_t sz);

	void CommitTransaction(uint32_t bs, uint32_t transactionId);

	Header* GetBlockHeader(void* startOfData);
	Footer* GetBlockFooter(void* startOfData);
	void* GetPreviousData(void* startOfData);

	bool Reclaim(Header* header, Footer* footer);
	bool TryReclaim(void* startOfData);

	template <typename T>
	T Align(T v, T alignment) 
	{
		return (v & (alignment - 1)) == 0
			? v
			: (v + alignment) & (~(alignment - 1));
	}

private:
	uint8_t *pool;
	std::atomic<uint32_t> tail;
	std::atomic<uint32_t> commit;

};

template <size_t SZ, uint8_t ALIGN>
SLStackAllocator<SZ, ALIGN>::SLStackAllocator()
	: pool{ new uint8_t[SZ] }
	, tail {0}
	, commit{0}
{}

template <size_t SZ, uint8_t ALIGN>
SLStackAllocator<SZ, ALIGN>::~SLStackAllocator()
{
	delete[] pool;
}

template <size_t SZ, uint8_t ALIGN>
void* SLStackAllocator<SZ, ALIGN>::alloc(size_t size)
{
	void* result = nullptr;

	size_t blockSize = size
		+ sizeof(SLStackAllocator::Header)
		+ sizeof(SLStackAllocator::Footer);
	
	std::optional<size_t> blockIndex = ReserveBlock(blockSize);
	
	if (blockIndex.has_value()) {
		InitBlock(*blockIndex, blockSize);
		CommitTransaction(blockSize, *blockIndex + blockSize);
		result = &pool[*blockIndex + sizeof(SLStackAllocator::Header)];
	}

	return result;
}

template <size_t SZ, uint8_t ALIGN>
void SLStackAllocator<SZ, ALIGN>::free(void* memory)
{
	uint8_t* asByteA = (uint8_t*)memory;
	
	SLStackAllocator::Header* header = GetBlockHeader(memory);
	SLStackAllocator::Footer* footer = GetBlockFooter(memory);

	header->locked = false;
	if (Reclaim(header, footer)) {
		void* previous = memory;
		
		do {
			previous = GetPreviousData(previous);
		} while (TryReclaim(previous));
	}
}

template <size_t SZ, uint8_t ALIGN>
std::optional<uint32_t> SLStackAllocator<SZ, ALIGN>::ReserveBlock(size_t size)
{
	uint32_t end;
	uint32_t blockBeginning = tail.load(std::memory_order_relaxed);;
	size = Align<size_t>(size, 4);

	do {
		end = blockBeginning + size;

		if (end > SZ)
		{
			return std::nullopt;
		}

	} while (!tail.compare_exchange_strong(blockBeginning, end));

	return blockBeginning;
}

template <size_t SZ, uint8_t ALIGN>
uint32_t SLStackAllocator<SZ, ALIGN>::InitBlock(uint32_t blockStart, size_t sz)
{
	Header* h = (Header*)&pool[blockStart];
	h->locked = true;
	h->size = sz;

	Footer* f = (Footer*)&pool[blockStart + sz - sizeof(SLStackAllocator::Footer)];
	f->blockStart = blockStart;

	return blockStart;
}

template <size_t SZ, uint8_t ALIGN>
void SLStackAllocator<SZ, ALIGN>::CommitTransaction(uint32_t bs, uint32_t transaction)
{
	while (true) {
		uint32_t desiredStage = transaction - bs;

		if (commit.compare_exchange_strong(desiredStage, transaction)) {
			break;
		}

		std::this_thread::sleep_for(std::chrono::nanoseconds{ 10 });
	}
}

template <size_t SZ, uint8_t ALIGN>
typename SLStackAllocator<SZ, ALIGN>::Header* SLStackAllocator<SZ, ALIGN>::GetBlockHeader(void* startOfData)
{
	uint8_t* asByteA = (uint8_t*)startOfData;
	SLStackAllocator<SZ, ALIGN>::Header* header =
		(SLStackAllocator<SZ, ALIGN>::Header*)(asByteA - sizeof(SLStackAllocator<SZ, ALIGN>::Header));

	return header;
}

template <size_t SZ, uint8_t ALIGN>
typename SLStackAllocator<SZ, ALIGN>::Footer* SLStackAllocator<SZ, ALIGN>::GetBlockFooter(void* startOfData)
{
	Header* h = GetBlockHeader(startOfData);
	uint8_t* asByteA = (uint8_t*)h;
	asByteA += h->size;
	asByteA -= sizeof(SLStackAllocator::Footer);

	return (SLStackAllocator::Footer*)asByteA;
}

template <size_t SZ, uint8_t ALIGN>
void* SLStackAllocator<SZ, ALIGN>::GetPreviousData(void* startOfData)
{
	uint8_t* asByteA = (uint8_t*)startOfData;
	asByteA -= sizeof(SLStackAllocator::Header);

	if (asByteA == pool)
	{
		return nullptr;
	}

	asByteA -= sizeof(SLStackAllocator::Footer);

	Footer* prevFooter = (Footer*)asByteA;
	return &pool[prevFooter->blockStart + sizeof(SLStackAllocator::Header)];
}

template <size_t SZ, uint8_t ALIGN>
bool SLStackAllocator<SZ, ALIGN>::Reclaim(Header* header, Footer* footer)
{
	uint32_t t = footer->blockStart + header->size;
	if (tail.compare_exchange_strong(t, footer->blockStart)) {
		commit = footer->blockStart;
		return true;
	}

	return false;
}

template <size_t SZ, uint8_t ALIGN>
bool SLStackAllocator<SZ, ALIGN>::TryReclaim(void* startOfData) {
	if (startOfData == nullptr) {
		return false;
	}

	SLStackAllocator::Header* header = GetBlockHeader(startOfData);
	SLStackAllocator::Footer* footer = GetBlockFooter(startOfData);

	if (header->locked) {
		return false;
	}

	return Reclaim(header, footer);
}