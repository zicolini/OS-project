//
// Created by os on 11/11/25.
//
#include "../h/print.hpp"
#include "../h/riscv.hpp"


void kprintString(const char *string) {
    //if(string)
    while(*string != '\0'){
        char volatile status = Riscv::r_consoleStatus();
        while(!(status & CONSOLE_TX_STATUS_BIT)){
            //update status
            status = Riscv::r_consoleStatus();
            //TCB::dispatch();
        }
        __asm__ volatile("sb %[character], (%[TXData])"
                ::
        [character] "r" (*string),
        [TXData] "r" (CONSOLE_TX_DATA)
        );
        string++;
    }
}

void kprintInt(uint64 integer) {
    char digits[11] = "0123456789";
    char string[20];
    int i = 0;
    do{
        string[i] = digits[integer % 10];
        integer /= 10;
        i++;
    }
    while(integer != 0);
    while(i){
        i--;
        char volatile status = Riscv::r_consoleStatus();
        while(!(status & CONSOLE_TX_STATUS_BIT)){
            //update status
            status = Riscv::r_consoleStatus();
            //TCB::dispatch();
        }
        __asm__ volatile("sb %[character], (%[TXData])"
                ::
        [character] "r" (string[i]),
        [TXData] "r" (CONSOLE_TX_DATA)
        );
    }
}
