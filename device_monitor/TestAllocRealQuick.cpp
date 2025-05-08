
#if 0

void TestAllocatorRealQuick()
{
    SLStackAllocator<1024, 8> alloc{};

    int* a = (int*)alloc.alloc(sizeof(int));
    {
        void* v = (void*)a;
        SLStackAllocator<1024, 8>::MemoryBlock block = SLStackAllocator<1024, 8>::MemoryBlock::FromStartOfData(v);
        auto* h = block.Header();
        auto* f = block.Footer();

        printf("a=%p hsz=%d fsz=%d t=%p c=%p h=%p f=%p\n", v, h->size, f->blockSize, (void*)alloc.tail.load(), (void*)alloc.commit.load(), h, f);
    }

    int* b = (int*)alloc.alloc(sizeof(int));
    {
        void* v = (void*)b;
        SLStackAllocator<1024, 8>::MemoryBlock block = SLStackAllocator<1024, 8>::MemoryBlock::FromStartOfData(v);
        auto* h = block.Header();
        auto* f = block.Footer();

        printf("b=%p hsz=%d fsz=%d t=%p c=%p h=%p f=%p\n", v, h->size, f->blockSize, (void*)alloc.tail.load(), (void*)alloc.commit.load(), h, f);
    }

    int* c = (int*)alloc.alloc(sizeof(int));
    {
        void* v = (void*)c;
        SLStackAllocator<1024, 8>::MemoryBlock block = SLStackAllocator<1024, 8>::MemoryBlock::FromStartOfData(v);
        auto* h = block.Header();
        auto* f = block.Footer();

        printf("c=%p hsz=%d fsz=%d t=%p c=%p h=%p f=%p\n", v, h->size, f->blockSize, (void*)alloc.tail.load(), (void*)alloc.commit.load(), h, f);
    }

    printf("t=%p c=%p\n", (void*)alloc.tail.load(), (void*)alloc.commit.load());
    alloc.free(b);
    printf("t=%p c=%p\n", (void*)alloc.tail.load(), (void*)alloc.commit.load());

    alloc.free(c);
    printf("t=%p c=%p\n", (void*)alloc.tail.load(), (void*)alloc.commit.load());

    alloc.free(a);
    printf("t=%p c=%p\n", (void*)alloc.tail.load(), (void*)alloc.commit.load());
}

#endif