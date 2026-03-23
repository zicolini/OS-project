//
// Created by os on 11/28/25.
//

#ifndef PROJECT_BASE_SEM_HPP
#define PROJECT_BASE_SEM_HPP

#include "List.hpp"
#include "TCB.hpp"
#include "../lib/mem.h"

class Sem {
public:


    static int openSem(Sem **handle, unsigned init){
         *handle = new Sem(init);   //no memory allocation in constructor, does not return errors
         return *handle ? 0 : -1;
    }

    static int closeSem(Sem *handle){
        delete handle;
        return 0;
    }

    int block();
    int unblock();
    int wait();
    int signal();

private:

    void *operator new(size_t n)
    {
        return MemoryAllocator::mem_alloc(n);
    }

    void *operator new[](size_t n)
    {
        return MemoryAllocator::mem_alloc(n);
    }

    void operator delete(void *p) noexcept
    {
        MemoryAllocator::mem_free(p);
    }

    void operator delete[](void *p) noexcept
    {
        MemoryAllocator::mem_free(p);
    }

    Sem(unsigned init): value(init), blockedHead(nullptr), blockedTail(nullptr){};
    ~Sem();
    void putBlocked(TCB* thread);
    TCB* getBlocked();
    //todo: razmotri vracanje na int za value type
    long value;
    TCB* blockedHead;
    TCB* blockedTail;
};



#endif //PROJECT_BASE_SEM_HPP
