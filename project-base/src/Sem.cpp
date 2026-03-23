//
// Created by os on 11/28/25.
//

#include "../h/Sem.hpp"
#include "../h/Scheduler.hpp"
#include "../h/riscv.hpp"
#include "../h/print.hpp"


Sem::~Sem() {
    while(TCB* thread = getBlocked()) {
        thread->setBlocked(false);
        Scheduler::put(thread);
    }
}

int Sem::block() {
    TCB::getRunning()->setBlocked(true);
    putBlocked(TCB::getRunning());
    uint64 volatile sstatus = Riscv::r_sstatus();   //needed for ConsoleDriver::printThread (sem called outside trapHandler)
    TCB::dispatch();
    Riscv::w_sstatus(sstatus);
    if(TCB::getRunning()->isBlocked()){             //released by destructor
        TCB::getRunning()->setBlocked(false);
        return -1;
    }
    return 0;
}

int Sem::unblock() {
    TCB* thread = getBlocked();
    if(!thread) return -1;
    thread->setBlocked(false);
    Scheduler::put(thread);
    return 0;
}

int Sem::wait() {
    if(--value < 0)
        return block();
    return 0;
}

int Sem::signal() {
    if(value++ < 0)
        return unblock();
    return 0;
}

void Sem::putBlocked(TCB *thread) {
    //thread->setNext(nullptr);
    if(blockedTail)
        blockedTail->setNext(thread);
    blockedTail = thread;
    if(!blockedHead)
        blockedHead = blockedTail;
}

TCB *Sem::getBlocked() {
    if (!blockedHead) {
        return nullptr;
    }
    TCB *removedThread = blockedHead;
    blockedHead = blockedHead->getNext();
    if (!blockedHead)
        blockedTail = blockedHead;
    removedThread->setNext(nullptr);
    return removedThread;
}

