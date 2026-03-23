//
// Created by os on 11/12/25.
//

#include "../h/riscv.hpp"
#include "../lib/console.h"
#include "../h/print.hpp"
#include "../h/TCB.hpp"
#include "../lib/mem.h"
#include "../h/MemoryAllocator.hpp"
#include "../h/Sem.hpp"
#include "../h/Scheduler.hpp"
#include "../h/ConsoleDriver.hpp"


void Riscv::sretToRa() {
    c_sstatus(SSTATUS_SPP);        //all created threads running in user mode
    Riscv::threadInit();
    return;
    __asm__ volatile ("csrw sepc, ra\n\t"
                      "sret");
}

int errorReported = 0;

void Riscv::trapHandler() {

    uint64 volatile ecallNumber;
    __asm__ volatile ("mv %0, a0" : "=r" (ecallNumber));
    uint64 volatile scause = r_scause();
    switch (scause) {
        case 0x0000000000000002UL:
            if(!errorReported) {
                kprintInt(TCB::running->getMyID());
                kprintString(" Illegal instruction ");
                //todo terminate OS
            }
            Riscv::c_sip(SIP_SSIP|SIP_SEIP);
            errorReported++;
            break;

        case 0x0000000000000005UL:
            if(!errorReported) {
                kprintInt(TCB::running->getMyID());
                kprintString(" Illegal read ");
            }
            errorReported++;
            break;

        case 0x0000000000000007UL:
            if(!errorReported) {
                kprintInt(TCB::running->getMyID());
                kprintString(" Illegal write ");
            }
            errorReported++;
            break;


        //ecall from user mode
        case 0x0000000000000008UL: {
            uint64 volatile sepc = r_sepc() + 4;        //ecall sets epc to ecall invocation address, to return to next instruction increment sepc by 4
            uint64 volatile sstatus = r_sstatus();
            Riscv::c_sip(SIP_SSIP);
            switch (ecallNumber) {

                case MEM_ALLOC: {
                    size_t size;
                    __asm__ volatile ("ld %0, 8 * 11(fp)" : "=r" (size));
                    void *addr = MemoryAllocator::mem_alloc(size);
                    __asm__ volatile ("mv a0, %0" : : "r" (addr));
                    __asm__ volatile ("sd a0, 8*10(fp)" : : );
                    break;
                }
                case MEM_FREE:{
                    void *address;
                    __asm__ volatile ("ld %0, 8 * 11(fp)" : "=r" (address));
                    int volatile retval = MemoryAllocator::mem_free(address);
                    __asm__ volatile ("mv a0, %0" : : "r" (retval));
                    __asm__ volatile ("sd a0, 8*10(fp)" : : );
                    break;
                }
                case MEM_GET_FREE_SPACE:{
                    size_t volatile space = MemoryAllocator::mem_get_free_space();
                    __asm__ volatile ("mv a0, %0" : : "r" (space));
                    __asm__ volatile ("sd a0, 8*10(fp)" : : );
                    break;
                }
                case MEM_GET_LARGEST_FREE_BLOCK:{
                    size_t volatile size = MemoryAllocator::mem_get_largest_free_block();
                    __asm__ volatile ("mv a0, %0" : : "r" (size));
                    __asm__ volatile ("sd a0, 8*10(fp)" : : );
                    break;
                }
                case THREAD_CREATE: {
                    TCB **handle;
                    void (*body)(void *);
                    void *args;
                    void *stack_space;
                    __asm__ volatile ("ld %0, 8 * 11(fp)\n\t"
                                      "ld %1, 8 * 12(fp)\n\t"
                                      "ld %2, 8 * 13(fp)\n\t"
                                      "ld %3, 8 * 14(fp)\n\t"
                                      : "=r" (handle),
                                        "=r" (body),
                                        "=r" (args),
                                        "=r" (stack_space));
                    int volatile returnCode = TCB::createThread(handle, body, args, stack_space);
                    __asm__ volatile ("mv a0, %0" : : "r" (returnCode));
                    __asm__ volatile ("sd a0, 8*10(fp)" : : );
                    break;
                }
                case THREAD_EXIT:
                    TCB::running->setFinished(true);
                    __asm__ volatile ("li a0, %0" : : "i" (-1)); //returning from thread_exit syscall happens only if an error has occurred
                    __asm__ volatile ("sd a0, 8*10(fp)" : : );
                    //fall through to dispatch
                case THREAD_DISPATCH: {
                    TCB::dispatch();
                    break;
                }
                case SEM_OPEN: {
                    Sem **handle;
                    unsigned init;
                    __asm__ volatile ("ld %0, 8 * 11(fp)\n\t"
                                      "ld %1, 8 * 12(fp)\n\t"
                                      :
                                      "=r" (handle),
                                      "=r" (init)
                                      );
                    int volatile returnCode = Sem::openSem(handle, init);
                    __asm__ volatile ("mv a0, %0" : : "r" (returnCode));
                    __asm__ volatile ("sd a0, 8*10(fp)" : : );
                    break;
                }
                case SEM_CLOSE: {
                    Sem *handle;
                    __asm__ volatile ("ld %0, 8 * 11(fp)\n\t"
                            :
                            "=r" (handle)
                            );
                    int volatile returnCode = Sem::closeSem(handle);
                    __asm__ volatile ("mv a0, %0" : : "r" (returnCode));
                    __asm__ volatile ("sd a0, 8*10(fp)" : : );
                    break;
                }
                case SEM_SIGNAL: {
                    Sem *id;
                    __asm__ volatile ("ld %0, 8 * 11(fp)\n\t"
                            :
                            "=r" (id)
                            );
                    int volatile returnCode = id->signal();
                    __asm__ volatile ("mv a0, %0" : : "r" (returnCode));
                    __asm__ volatile ("sd a0, 8*10(fp)" : : );
                    break;
                }
                case SEM_WAIT: {
                    Sem *id;
                    __asm__ volatile ("ld %0, 8 * 11(fp)\n\t"
                            :
                            "=r" (id)
                            );
                    int volatile returnCode = id->wait();
                    __asm__ volatile ("mv a0, %0" : : "r" (returnCode));
                    __asm__ volatile ("sd a0, 8*10(fp)" : : );
                    break;
                }
                case TIME_SLEEP: {
                    time_t duration;
                    __asm__ volatile ("ld %0, 8 * 11(fp)\n\t"
                            :
                            "=r" (duration)
                            );
                    int volatile returnCode = Scheduler::putSleeping(TCB::running, duration);
                    TCB::dispatch();
                    __asm__ volatile ("mv a0, %0" : : "r" (returnCode));
                    __asm__ volatile ("sd a0, 8*10(fp)" : : );
                    break;
                }

                case GETC:{
                    char volatile character = ConsoleDriver::readBufferGet();
                    __asm__ volatile ("mv a0, %0" : : "r" (character));
                    __asm__ volatile ("sd a0, 8*10(fp)" : : );
                    break;
                }
                case PUTC:{
                    char volatile character;
                    __asm__ volatile ("ld %0, 8 * 11(fp)\n\t"
                            :
                            "=r" (character)
                            );
                    ConsoleDriver::printBufferPut(character);
                    break;
                }
                default:
                    kprintInt(TCB::running->getMyID());
                    kprintString(" Nepostojeci ecall");
                    break;
            }

            w_sstatus(sstatus);
            w_sepc(sepc);
            break;
        }

        //ecall from supervisor mode, should not happen
        case 0x0000000000000009UL:
        {
            kprintInt(TCB::running->getMyID());
            kprintString(" Ecall from supervisor mode\n");
            break;
        }

        //machine timer interrupt
        case 0x8000000000000001UL: {
            //kprintString("timer interrupt ");
            Riscv::c_sip(SIP_SSIP);
            Scheduler::timerUpdate();
            TCB::timeSliceCounter++;
            if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
            {
                uint64 volatile sepc = r_sepc();
                uint64 volatile sstatus = r_sstatus();
                TCB::dispatch();
                w_sstatus(sstatus);
                w_sepc(sepc);
                break;
            }
            break;
        }

        //console interrupt
        case 0x8000000000000009UL: {
            //todo: clear interrupt pending
            char character;
            char status;
            if(plic_claim() == CONSOLE_IRQ) {
                Riscv::c_sip(SIP_SEIP);
                status = Riscv::r_consoleStatus();
                //int i = 0;
                while((status & CONSOLE_RX_STATUS_BIT) && ConsoleDriver::readBufferAvailable()){
                    status = Riscv::r_consoleStatus();              //update status
                    __asm__ volatile("lb %[character], (%[RXData])" //get character from controller buffer
                                    :
                                    [character] "=r" (character)
                                    :
                                    [RXData] "r" (CONSOLE_RX_DATA)
                                    );
                    if(character)
                        ConsoleDriver::readBufferPut(character);
                }
                plic_complete(CONSOLE_IRQ);
            }
            break;
        }

        default:
            kprintInt(TCB::running->getMyID());
            kprintString(" unexpected interrupt ");
            kprintInt(scause);
            break;
    }
}