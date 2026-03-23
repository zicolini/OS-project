//
// Created by os on 11/8/25.
//

#include "../h/MemoryAllocator.hpp"


bool MemoryAllocator::initialized = false;
size_t MemoryAllocator::largestFreeBlock;
size_t MemoryAllocator::freeSpace;
BlockHeader* MemoryAllocator::freeMemHead;

//first fit
void *MemoryAllocator::mem_alloc(size_t size) {
    size_t allocatedSize = ((size + sizeof(size_t) + MEM_BLOCK_SIZE - 1)/MEM_BLOCK_SIZE ) * MEM_BLOCK_SIZE;
    if(allocatedSize > largestFreeBlock)
        return nullptr;
    BlockHeader* prev = nullptr;
    BlockHeader* curr = freeMemHead;
    while (curr->size < allocatedSize) {
        prev = curr;
        curr = curr->next;
    }

    if(curr->size - allocatedSize >= MEM_BLOCK_SIZE) {
        BlockHeader *newBlockHeader = (BlockHeader *) ((char *) curr + allocatedSize);
        newBlockHeader->size = curr->size - allocatedSize;
        newBlockHeader->next = curr->next;
        if (prev)
            prev->next = newBlockHeader;
        else freeMemHead = newBlockHeader;
    }
    else{
        if(prev)
            prev->next = curr->next;
        else freeMemHead = curr->next;
    }

    size_t* sizePtr = (size_t*)curr;
    *sizePtr = allocatedSize;
    return (void*)curr;
}

int MemoryAllocator::mem_free(void * addr) {
    //todo dodaj if addr is nullptr
    BlockHeader* curr = nullptr;
    size_t freedSize = *((size_t*)addr);
    if (addr<freeMemHead || freeMemHead == nullptr){
        BlockHeader* newBlockHeader = (BlockHeader*)addr;
        curr = newBlockHeader;
        curr->size = freedSize;
        curr->next = freeMemHead;
        freeMemHead = curr;
    }
    else {
        curr = freeMemHead;
        while(curr->next && curr->next < addr)
            curr = curr->next;
        if((char*)curr + curr->size == addr){
            curr->size += freedSize;
        }
        else{
            BlockHeader* newBlockHeader = (BlockHeader*)addr;
            newBlockHeader->next = curr;
            newBlockHeader->size = freedSize;
            curr = newBlockHeader;
        }
    }

    if((char*)curr + curr->size == (char*)curr->next){
        curr->size += curr->next->size;
        curr->next = curr->next->next;
    }

    if(curr->size > largestFreeBlock)
        largestFreeBlock = curr->size;

    freeSpace+= freedSize;
    return 0;
}

size_t MemoryAllocator::mem_get_free_space() {
    return freeSpace;
}

size_t MemoryAllocator::mem_get_largest_free_block() {
    return largestFreeBlock;
}

/*
 * see init() for usage
    struct dummyStruct{
    char array[MEM_BLOCK_SIZE];
};*/

void MemoryAllocator::init() {
    //const void* alignedHeapStart = (dummyStruct*)HEAP_START_ADDR; use to align
    if(initialized)
        return;
    initialized = true;
    freeMemHead = (BlockHeader*)HEAP_START_ADDR; //alignment necessary?
    freeMemHead->next = nullptr;
    freeMemHead->size = freeSpace = largestFreeBlock = (char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR;
}
