//
// Created by os on 11/13/25.
//

#ifndef PROJECT_BASE_SCHEDULER_HPP
#define PROJECT_BASE_SCHEDULER_HPP

#include "List.hpp"
#include "TCB.hpp"

class Scheduler{
    static TCB* readyHead;
    static TCB* readyTail;
    static TCB* sleepingHead;
public:
    static void put(TCB* thread);
    static TCB* get();
    static int putSleeping(TCB* thread, time_t duration);
    static void timerUpdate();
};

#endif //PROJECT_BASE_SCHEDULER_HPP
