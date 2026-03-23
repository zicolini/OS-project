//
// Created by os on 11/11/25.
//

#ifndef PROJECT_BASE_RISCV_HPP
#define PROJECT_BASE_RISCV_HPP

#include "../lib/hw.h"

class Riscv{

public:

    static void trap();
    static void sretToRa();
    static void threadInit();

    static uint64 r_scause();
    static void w_scause(uint64 scause);

    static uint64 r_stvec();
    static void w_stvec(uint64 stvec);

    static uint64 r_sepc();
    static void w_sepc(uint64 sepc);

    static uint64 r_stval();
    static void w_stval(uint64 stval);

    enum sipMask{
        SIP_SSIP = (1 << 1),
        SIP_STIP = (1 << 5),
        SIP_SEIP = (1 << 9)
    };
    static uint64 r_sip();
    static void w_sip(uint64 sip);
    static void s_sip(uint64 mask);
    static void c_sip(uint64 mask);

    enum sieMask{
        SIE_SSIE = (1 << 1),
        SIE_STIE = (1 << 5),
        SIE_SEIE = (1 << 9)
    };
    static uint64 r_sie();
    static void w_sie(uint64 sie);
    static void s_sie(uint64 mask);
    static void c_sie(uint64 mask);

    enum sstatusMask{
        SSTATUS_SIE = (1 << 1),
        SSTATUS_SPIE = (1 << 5),
        SSTATUS_SPP = (1 << 8)
    };
    static uint64 r_sstatus();
    static void w_sstatus(uint64 sstatus);
    static void s_sstatus(uint64 mask);
    static void c_sstatus(uint64 mask);


    enum ecallEnum{
        MEM_ALLOC                   = 0x01,
        MEM_FREE                    = 0x02,
        MEM_GET_FREE_SPACE          = 0x03,
        MEM_GET_LARGEST_FREE_BLOCK  = 0x04,
        THREAD_CREATE               = 0x11,
        THREAD_EXIT                 = 0x12,
        THREAD_DISPATCH             = 0x13,
        SEM_OPEN                    = 0x21,
        SEM_CLOSE                   = 0x22,
        SEM_WAIT                    = 0x23,
        SEM_SIGNAL                  = 0x24,
        TIME_SLEEP                  = 0x31,
        GETC                        = 0x41,
        PUTC                        = 0x42
    };

    static char r_consoleStatus();

private:
    static void trapHandler();

};

inline uint64 Riscv::r_scause(){
    uint64 volatile scause;
    __asm__ volatile ("csrr %[scause], scause"  : [scause] "=r"(scause));
    return scause;
}

inline void Riscv::w_scause(uint64 scause) {
    __asm__ volatile ("csrw scause, %[scause]" : : [scause] "r"(scause));
}

inline uint64 Riscv::r_stvec(){
    uint64 volatile stvec;
    __asm__ volatile ("csrr %[stvec], stvec"  : [stvec] "=r"(stvec));
    return stvec;
}

inline void Riscv::w_stvec(uint64 stvec) {
    __asm__ volatile ("csrw stvec, %[stvec]" : : [stvec] "r"(stvec));
}

inline uint64 Riscv::r_sepc(){
    uint64 volatile sepc;
    __asm__ volatile ("csrr %[sepc], sepc"  : [sepc] "=r"(sepc));
    return sepc;
}

inline void Riscv::w_sepc(uint64 sepc) {
    __asm__ volatile ("csrw sepc, %[sepc]" : : [sepc] "r"(sepc));
}

inline uint64 Riscv::r_stval(){
    uint64 volatile stval;
    __asm__ volatile ("csrr %[stval], stval"  : [stval] "=r"(stval));
    return stval;
}

inline void Riscv::w_stval(uint64 stval) {
    __asm__ volatile ("csrw stval, %[stval]" : : [stval] "r"(stval));
}

inline uint64 Riscv::r_sip(){
    uint64 volatile sip;
    __asm__ volatile ("csrr %[sip], sip"  : [sip] "=r"(sip));
    return sip;
}

inline void Riscv::w_sip(uint64 sip) {
    __asm__ volatile ("csrw sip, %[sip]" : : [sip] "r"(sip));
}

inline void Riscv::s_sip(uint64 mask) {
    __asm__ volatile ("csrs sip, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::c_sip(uint64 mask) {
    __asm__ volatile ("csrc sip, %[mask]" : : [mask] "r"(mask));
}

inline uint64 Riscv::r_sie(){
    uint64 volatile sie;
    __asm__ volatile ("csrr %[sie], sie"  : [sie] "=r"(sie));
    return sie;
}

inline void Riscv::s_sie(uint64 mask) {
    __asm__ volatile ("csrs sie, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::c_sie(uint64 mask) {
    __asm__ volatile ("csrc sie, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::w_sie(uint64 sie) {
    __asm__ volatile ("csrw sie, %[sie]" : : [sie] "r"(sie));
}

inline uint64 Riscv::r_sstatus(){
    uint64 volatile sstatus;
    __asm__ volatile ("csrr %[sstatus], sstatus"  : [sstatus] "=r"(sstatus));
    return sstatus;
}

inline void Riscv::w_sstatus(uint64 sstatus) {
    __asm__ volatile ("csrw sstatus, %[sstatus]" : : [sstatus] "r"(sstatus));
}

inline void Riscv::s_sstatus(uint64 mask) {
    __asm__ volatile ("csrs sstatus, %[mask]" : : [mask] "r"(mask));
}

inline void Riscv::c_sstatus(uint64 mask) {
    __asm__ volatile ("csrc sstatus, %[mask]" : : [mask] "r"(mask));
}

inline char Riscv::r_consoleStatus(){
    char volatile cStatus;
    __asm__ volatile("lb %[status], (%[consoleStatusAddress])"
            :
    [status] "=r" (cStatus)
    :
    [consoleStatusAddress] "r" (CONSOLE_STATUS)
    );
    return cStatus;
}




#endif //PROJECT_BASE_RISCV_HPP
