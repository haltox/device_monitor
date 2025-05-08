#pragma once

#include <atomic>
#include <stdint.h>
#include <optional>
#include <thread>
#include <bit>

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

public:

	struct Header
	{
		std::atomic<uint8_t> locked;
		uint32_t size;
	};

	struct Footer
	{
		uint32_t blockSize;
	};

	struct MemoryBlock 
	{
		MemoryBlock() {}
		MemoryBlock(uint8_t* ptr) : startOfBlock{ ptr } {}

		uint8_t* startOfBlock {nullptr};

		MemoryBlock Init(size_t sz);

		Header* Header();
		Footer* Footer();
		uint8_t* StartOfData();
		MemoryBlock PreviousBlock();

		uint32_t GetBlockSize();

		static MemoryBlock FromStartOfData(void* startOfData);
	};

//private:
	std::optional<MemoryBlock> ReserveBlock(size_t sz);

	bool hasPreviousBlock(MemoryBlock block);
	void CommitBlock(MemoryBlock block);

	bool Reclaim(MemoryBlock block);
	bool TryReclaim(MemoryBlock block);

	template <typename T>
	T Align(T v, T alignment) 
	{
		return (v & (alignment - 1)) == 0
			? v
			: (v + alignment) & (~(alignment - 1));
	}

//private:
	uint8_t *pool;
	std::atomic<uint64_t> tail;
	std::atomic<uint64_t> commit;
};

template <size_t SZ, uint8_t ALIGN>
SLStackAllocator<SZ, ALIGN>::SLStackAllocator()
	: pool{ new uint8_t[SZ] }
	, tail { std::bit_cast<uint64_t, uint8_t*>(pool) }
	, commit{ std::bit_cast<uint64_t, uint8_t*>(pool) }
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
	
	std::optional<MemoryBlock> block = ReserveBlock(blockSize);
	
	if (block.has_value()) {
		block->Init(blockSize);
		CommitBlock(*block);
		result = (void*)block->StartOfData();
	}

	return result;
}

template <size_t SZ, uint8_t ALIGN>
void SLStackAllocator<SZ, ALIGN>::free(void* memory)
{
	SLStackAllocator::MemoryBlock block = SLStackAllocator::MemoryBlock::FromStartOfData(memory);
	SLStackAllocator::Header* header = block.Header();

	header->locked = false;
	if (Reclaim(block)) {
		while (hasPreviousBlock(block)) {
			block = block.PreviousBlock();
			if (!TryReclaim(block)) {
				break;
			}
		}
	}
}

template <size_t SZ, uint8_t ALIGN>
std::optional<typename SLStackAllocator<SZ, ALIGN>::MemoryBlock> SLStackAllocator<SZ, ALIGN>::ReserveBlock(size_t size)
{
	uint64_t poolEnd = ( std::bit_cast<uint64_t, uint8_t*>(pool) + SZ);
	uint64_t end;
	uint64_t blockAddr = tail.load(std::memory_order_relaxed);
	size = Align<size_t>(size, 4);

	do {
		end = blockAddr + size;

		if (end > poolEnd)
		{
			return std::nullopt;
		}

	} while (!tail.compare_exchange_strong(blockAddr, end));

	return MemoryBlock{ std::bit_cast<uint8_t*, uint64_t>(blockAddr) };
}

template <size_t SZ, uint8_t ALIGN>
bool SLStackAllocator<SZ, ALIGN>::hasPreviousBlock(MemoryBlock block)
{
	return block.startOfBlock > pool;
}

template <size_t SZ, uint8_t ALIGN>
void SLStackAllocator<SZ, ALIGN>::CommitBlock(MemoryBlock block)
{
	while (true) {
		uint64_t baseStage = std::bit_cast<uint64_t, uint8_t*>(block.startOfBlock);
		uint64_t blockCommit = baseStage + block.GetBlockSize();

		if (commit.compare_exchange_strong(baseStage, blockCommit)) {
			break;
		}

		std::this_thread::sleep_for(std::chrono::nanoseconds{ 10 });
	}
}

template <size_t SZ, uint8_t ALIGN>
bool SLStackAllocator<SZ, ALIGN>::Reclaim(MemoryBlock block)
{
	SLStackAllocator::Header *header = block.Header();
	SLStackAllocator::Footer *footer= block.Footer();

	uint64_t reclaimedTail = std::bit_cast<uint64_t, uint8_t*>(block.startOfBlock);
	uint64_t expectedTail = reclaimedTail + block.GetBlockSize();
	
	if (tail.compare_exchange_strong(expectedTail, reclaimedTail)) {
		commit = reclaimedTail;
		return true;
	}

	return false;
}

template <size_t SZ, uint8_t ALIGN>
bool SLStackAllocator<SZ, ALIGN>::TryReclaim(MemoryBlock block) {
	if (block.Header()->locked) {
		return false;
	}

	return Reclaim(block);
}


/////
template <size_t SZ, uint8_t ALIGN>
typename SLStackAllocator<SZ, ALIGN>::MemoryBlock SLStackAllocator<SZ, ALIGN>::MemoryBlock::Init(size_t sz)
{
	SLStackAllocator::Header* h = Header();
	h->locked = true;
	h->size = sz;

	SLStackAllocator::Footer* f = Footer();
	f->blockSize = sz;

	return *this;
}

template <size_t SZ, uint8_t ALIGN>
typename SLStackAllocator<SZ, ALIGN>::Header* SLStackAllocator<SZ, ALIGN>::MemoryBlock::Header()
{
	return (SLStackAllocator<SZ, ALIGN>::Header*)startOfBlock;
}

template <size_t SZ, uint8_t ALIGN>
typename SLStackAllocator<SZ, ALIGN>::Footer* SLStackAllocator<SZ, ALIGN>::MemoryBlock::Footer()
{
	SLStackAllocator<SZ, ALIGN>::Header* header = Header();

	uint8_t* asByteA = startOfBlock;
	asByteA += header->size;
	asByteA -= sizeof(SLStackAllocator<SZ, ALIGN>::Footer);
	return (SLStackAllocator<SZ, ALIGN>::Footer*)asByteA;
}

template <size_t SZ, uint8_t ALIGN>
uint8_t* SLStackAllocator<SZ, ALIGN>::MemoryBlock::StartOfData()
{
	uint8_t* asByteA = startOfBlock;
	asByteA += sizeof(SLStackAllocator<SZ, ALIGN>::Header);
	return asByteA;
}

template <size_t SZ, uint8_t ALIGN>
typename SLStackAllocator<SZ, ALIGN>::MemoryBlock SLStackAllocator<SZ, ALIGN>::MemoryBlock::PreviousBlock()
{
	uint8_t* asByteA = startOfBlock;
	asByteA -= sizeof(SLStackAllocator::Footer);

	SLStackAllocator::Footer* footer = (SLStackAllocator::Footer*)asByteA;
	asByteA -= footer->blockSize;
	asByteA += sizeof(SLStackAllocator::Footer);

	return SLStackAllocator::MemoryBlock{ asByteA };
}

template <size_t SZ, uint8_t ALIGN>
uint32_t SLStackAllocator<SZ, ALIGN>::MemoryBlock::GetBlockSize()
{
	SLStackAllocator<SZ, ALIGN>::Header* header = Header();
	return header->size;
}

template <size_t SZ, uint8_t ALIGN>
typename SLStackAllocator<SZ, ALIGN>::MemoryBlock SLStackAllocator<SZ, ALIGN>::MemoryBlock::FromStartOfData(void* startOfData)
{
	uint8_t* asByteA = (uint8_t*)startOfData;
	asByteA -= sizeof(SLStackAllocator::Header);

	return MemoryBlock{ asByteA };
}
