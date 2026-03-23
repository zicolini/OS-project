#include "../h/Scheduler.hpp"

//
// Created by os on 11/13/25.
//
//List<TCB> Scheduler::readyQueue;
TCB* Scheduler::readyHead = nullptr;
TCB *Scheduler::readyTail = nullptr;
TCB *Scheduler::sleepingHead = nullptr;

void Scheduler::put(TCB *thread) {
    thread->setNext(nullptr);
    if(readyTail)
        readyTail->setNext(thread);
    readyTail = thread;
    if(!readyHead)
        readyHead = readyTail;
}

TCB *Scheduler::get() {
    if (!readyHead) {
        return nullptr;
    }
    TCB *removedThread = readyHead;
    readyHead = readyHead->getNext();
    if (!readyHead)
        readyTail = readyHead;
    removedThread->setNext(nullptr);
    return removedThread;
}

int Scheduler::putSleeping(TCB *thread, time_t duration) {
    if(!thread) return -1;
    if (!duration) return 0;
    thread->setNext(nullptr);
    thread->setBlocked(true);
    TCB* prev = nullptr;
    TCB* curr = sleepingHead;
    time_t totalTime = 0;
    while(curr && (totalTime + curr->getTimeDelta() <= duration)){
        totalTime += curr->getTimeDelta();
        prev = curr;
        curr = curr->getNext();
    }
    time_t delta = duration - totalTime;
    thread->setTimeDelta(delta);
    if(prev)
        prev->setNext(thread);
    else
        sleepingHead = thread;
    if(curr) {
        thread->setNext(curr);
        curr->setTimeDelta(curr->getTimeDelta() - delta);
    }
    return 0;
}

void Scheduler::timerUpdate() {
    if (!sleepingHead)
        return;
    sleepingHead->decTimeDelta();
    while(sleepingHead && !sleepingHead->getTimeDelta()){
        TCB *unblockedThread = sleepingHead;
        sleepingHead = sleepingHead->getNext();
        unblockedThread->setBlocked(false);
        put(unblockedThread);
    }
}


