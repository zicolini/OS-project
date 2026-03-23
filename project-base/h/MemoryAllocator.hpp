//
// Created by os on 11/8/25.
//

#ifndef PROJECT_BASE_MEMORYALLOCATOR_HPP
#define PROJECT_BASE_MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

struct BlockHeader{
    BlockHeader* next;
    size_t size;
};



class MemoryAllocator{

private:
    //todo: razmotri zamenu init() f-je inicijalizacijom statickih promenljivih u .cpp fajlu
    static size_t largestFreeBlock;
    static size_t freeSpace;
    static BlockHeader* freeMemHead;
    static bool initialized;

public:
    MemoryAllocator() = delete;
    static void init();
    static void* mem_alloc(size_t size);
    static int mem_free(void* addr);
    static size_t mem_get_free_space();
    static size_t mem_get_largest_free_block();
};


#endif //PROJECT_BASE_MEMORYALLOCATOR_HPP
