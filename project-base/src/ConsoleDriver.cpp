//
// Created by os on 12/1/25.
//

#include "../h/ConsoleDriver.hpp"
#include "../h/Sem.hpp"
#include "../lib/hw.h"
#include "../h/riscv.hpp"

char ConsoleDriver::printBuffer[BUFFER_SIZE];
char ConsoleDriver::readBuffer[BUFFER_SIZE];
int ConsoleDriver::printHead = 0;
int ConsoleDriver::printTail = 0;
int ConsoleDriver::readHead = 0;
int ConsoleDriver::readTail = 0;
Sem *ConsoleDriver::printItemAvailable;
Sem *ConsoleDriver::printSpaceAvailable;
Sem *ConsoleDriver::readItemAvailable;

Sem *ConsoleDriver::printMutex;

ConsoleDriver::ConsoleDriver() {

}

void ConsoleDriver::ConsoleInit() {
    Sem::openSem(&printSpaceAvailable, BUFFER_CNT);
    Sem::openSem(&printItemAvailable, 0);
    Sem::openSem(&readItemAvailable, 0);

    Sem::openSem(&printMutex, 1);
}

void ConsoleDriver::printThread(void *arg) {
    char c;
    char volatile status;
    //todo consider removing csstatus as its already done in threadWrapper
    //Riscv::c_sstatus(Riscv::SSTATUS_SIE);
    while(true){
        c = printBufferGet();
        status = Riscv::r_consoleStatus();
        while(!(status & CONSOLE_TX_STATUS_BIT)){
            //update status
            status = Riscv::r_consoleStatus();
            //TCB::dispatch();
        }
        __asm__ volatile("sb %[character], (%[TXData])"
                        ::
                        [character] "r" (c),
                        [TXData] "r" (CONSOLE_TX_DATA)
                        );
    }
}


//called from putc()
void ConsoleDriver::printBufferPut(char c) {
    volatile char character = c;
    printSpaceAvailable->wait();
    //todo consider removing print mutex, as its probably not needed
        //printMutex->wait();
    printBuffer[printTail] = character;
    printTail = (printTail + 1) % BUFFER_SIZE;
        //printMutex->signal();
    printItemAvailable->signal();
}

//called only from printThread
char ConsoleDriver::printBufferGet() {
    printItemAvailable->wait();
        //printMutex->wait();
    int ret = printBuffer[printHead];
    printHead = (printHead + 1) % BUFFER_SIZE;
        //printMutex->signal();
    printSpaceAvailable->signal();
    return ret;
}

//called from console interrupt
void ConsoleDriver::readBufferPut(char c) {
        //readTailMutex->wait();
    readBuffer[readTail] = c;
    readTail = (readTail + 1) % BUFFER_SIZE;
        //readTailMutex->signal();
    readItemAvailable->signal();
}

//called from getc()
char ConsoleDriver::readBufferGet() {
    readItemAvailable->wait();
        //readHeadMutex->wait();
    int ret = readBuffer[readHead];
    readHead = (readHead + 1) % BUFFER_SIZE;
        //readHeadMutex->signal();
    return ret;
}

//called from console interrupt
bool ConsoleDriver::readBufferAvailable() {
        //readHeadMutex->wait();
    int count = (BUFFER_SIZE + readTail - readHead) % BUFFER_SIZE;
        //readHeadMutex->signal();
    return count < BUFFER_CNT;

}
