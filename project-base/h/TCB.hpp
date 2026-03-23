//
// Created by os on 11/13/25.
//

#ifndef PROJECT_BASE_TCB_HPP
#define PROJECT_BASE_TCB_HPP


#include "../lib/hw.h"
#include "../lib/mem.h"
#include "MemoryAllocator.hpp"

class TCB{

    friend class Riscv;

public:

    static int id;

    enum type{
        kernel = 0,
        user = 1
    };

    static TCB* running;
    static uint64 timeSliceCounter;

    ~TCB(){
        //todo: razvezati ulancanu listu kojoj pripada tekuca nit (thread_exit?)
        MemoryAllocator::mem_free(stack);
        MemoryAllocator::mem_free(kstack);
    }

    using Body = void (*)(void*);

    static int createMainThread(TCB **handle){
        new TCB(handle,DEFAULT_TIME_SLICE);
        return 0;
    }

    static int createThread(TCB **handle, Body body, void *args, void *stack_space, type type = user){
        TCB* thread = new TCB(handle, body, args, DEFAULT_TIME_SLICE, stack_space, type);
        return (!thread || thread->finished) ? -1 : 0;
    }

    static void dispatch();

    bool isFinished() const {
        return finished;
    }
    void setFinished(bool setVal) {
        finished = setVal;
    }

    bool isBlocked() const{
        return blocked;
    }
    void setBlocked(bool setVal) {
        blocked = setVal;
    }

    static TCB *getRunning() {
        return TCB::running;
    }
    static void setRunning(TCB *setVal) {
        TCB::running = setVal;
    }

    uint64 getTimeSlice() const{
        return timeSlice;
    }

    TCB *getNext() const{
        return next;
    }
    void setNext(TCB *ptr){
        this->next = ptr;
    }

    time_t getTimeDelta() const{
        return timeDelta;
    }
    void setTimeDelta(time_t delta){
        this->timeDelta = delta;
    }
    void decTimeDelta(){
        this->timeDelta--;
    }

    type getType() const{
        return myType;
    }

    int getMyID() const{
        return myID;
    }

private:

    void *operator new(size_t n)
    {
        return MemoryAllocator::mem_alloc(n);
    }

    void *operator new[](size_t n)
    {
        return MemoryAllocator::mem_alloc(n);
    }

    void operator delete(void *p)
    {
        MemoryAllocator::mem_free(p);
    }

    void operator delete[](void *p)
    {
        MemoryAllocator::mem_free(p);
    }

    TCB(TCB **handle, uint64 timeSlice);

    TCB(TCB **handle, Body body, void *args, uint64 timeSlice, void *stack_space, type type = user);

    struct Context
    {
        uint64 ra;
        uint64 sp;
        uint64 ssp;
    };
    Context context;
    Body body;
    void *args;
    uint64 *stack;
    uint64 *kstack;
    uint64 timeSlice;
    bool finished;
    bool blocked;
    time_t timeDelta; //time to wait in sleeping queue on top of total sleep time of preceding sleeping threads
    int myID;
    type myType;
    TCB* next;

    static void threadWrapper();
    static void contextSwitch(Context *oldContext, Context *runningContext);

};


#endif //PROJECT_BASE_TCB_HPP
