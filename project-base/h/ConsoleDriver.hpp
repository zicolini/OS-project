//
// Created by os on 12/1/25.
//

#ifndef PROJECT_BASE_CONSOLEDRIVER_HPP
#define PROJECT_BASE_CONSOLEDRIVER_HPP


#include "Sem.hpp"


class ConsoleDriver {
    static int const BUFFER_CNT = 127;
    static int const BUFFER_SIZE = BUFFER_CNT + 1;
    static int printHead, printTail, readHead, readTail;
    static char printBuffer[BUFFER_SIZE], readBuffer[BUFFER_SIZE];
    static Sem *printSpaceAvailable, *printItemAvailable, *readItemAvailable;

    static Sem *printMutex;

    friend class Riscv;

public:

    ConsoleDriver();
    static void ConsoleInit();

    static void printBufferPut(char c);
    static char printBufferGet();
    static void printThread(void* arg);

    static void readBufferPut(char c);
    static char readBufferGet();
    static bool readBufferAvailable();

};


#endif //PROJECT_BASE_CONSOLEDRIVER_HPP
