//
// Created by os on 11/13/25.
//

#include "../h/TCB.hpp"
#include "../h/Scheduler.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"
#include "../lib/mem.h"
#include "../h/print.hpp"
#include "../test/printing.hpp"


TCB *TCB::running = nullptr;
uint64 TCB::timeSliceCounter = 0;
int TCB::id = 0;

//constructor used for main thread only, called from static createMainThread
TCB::TCB(TCB **handle, uint64 timeSlice) :
    body(nullptr),
    args(nullptr),
    kstack(nullptr),
    timeSlice(timeSlice),
    finished(false),
    blocked(false),
    timeDelta(0),
    myID(0),
    next(nullptr)
{
    //stack = (uint64 *) __mem_alloc(DEFAULT_STACK_SIZE);
    context = {(uint64 )nullptr,
               0,
               0};
    running = this;
    *handle = this;

}


TCB::TCB(TCB **handle, TCB::Body body, void *args, uint64 timeSlice, void* stack, type type) :
        body(body),
        args(args),
        stack((uint64 *)stack),
        timeSlice(timeSlice),
        finished(false),
        blocked(false),
        timeDelta(0),
        myID(++id),
        myType(type),
        next(nullptr)
{
    kstack = (uint64 *) MemoryAllocator::mem_alloc(DEFAULT_STACK_SIZE);
        if(!kstack) finished = true;
        //todo put in finished queue
    context = {(uint64)&threadWrapper,
               (uint64) &this->stack[DEFAULT_STACK_SIZE/8],  //not universally correct
               (uint64) &kstack[DEFAULT_STACK_SIZE/8]
    };
    *handle = this;
        if(finished) return;
    Scheduler::put(this);
}

void TCB::threadWrapper() {
    if(running->getType() == user) {
        Riscv::c_sstatus(Riscv::SSTATUS_SPP);
        Riscv::s_sstatus(Riscv::SSTATUS_SPIE);
    }
    else
        Riscv::s_sstatus(Riscv::SSTATUS_SPP);
    Riscv::threadInit();
    running->body(running->args);
    thread_exit();  //syscall - threadWrapper should be running in user mode
}


void TCB::dispatch() {
    TCB::timeSliceCounter = 0;
    TCB *old = running;
    if (!old->isFinished() && !old->isBlocked()) { Scheduler::put(old); }
    running = Scheduler::get();
//    kprintInt(old->myID);
//    kprintString("->");
//    kprintInt(running->myID);
//    kprintString("\n");
    TCB::contextSwitch(&old->context, &running->context);
}







