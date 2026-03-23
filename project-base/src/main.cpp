#include "../lib/hw.h"
#include "../lib/console.h"
#include "../h/print.hpp"
#include "../lib/mem.h"
#include "../h/riscv.hpp"
#include "../h/TCB.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/userMain.hpp"
#include "../h/ConsoleDriver.hpp"

#include "../h/syscall_c.hpp"

void testingThread(void *arg){
    //char i  = 0;

    while(0){
        char c = getc();
        putc(c);
    }
    userMain();
}

void main(){
    //Riscv::s_sstatus(Riscv::SSTATUS_SIE);
    Riscv::w_stvec((uint64) &Riscv::trap);
    MemoryAllocator::init();
    TCB* mainThread;
    TCB::createMainThread(&mainThread);
    void* printThreadStack = MemoryAllocator::mem_alloc(DEFAULT_STACK_SIZE);
    void* userMainStack = MemoryAllocator::mem_alloc(DEFAULT_STACK_SIZE);
    TCB* userThread;
    TCB* printThread;
    TCB::createThread(&printThread, ConsoleDriver::printThread, nullptr, printThreadStack, TCB::kernel);
    TCB::createThread(&userThread, testingThread, nullptr, userMainStack);
    ConsoleDriver::ConsoleInit();
    while(!userThread->isFinished()) {
        Riscv::c_sstatus(Riscv::SSTATUS_SIE);
        uint64 volatile sstatus = Riscv::r_sstatus();
        TCB::dispatch();
        Riscv::w_sstatus(sstatus);
        Riscv::s_sstatus(Riscv::SSTATUS_SIE);
    }
    //kprintString("MT ended\n");
    //delete userMainThread;
    //kprintString("UMT deleted\n");
    //delete mainThread;
}
