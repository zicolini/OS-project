//
// Created by os on 11/13/25.
//
#include "../h/syscall_c.hpp"
#include "../h/riscv.hpp"
#include "../lib/hw.h"


//triggers ecall with call number in a0,
//and arguments in following regs
//retval in a0

void *mem_alloc(size_t size) {
    //TODO: conversion to blocks
    //size = (size + MEM_BLOCK_SIZE - 1)/MEM_BLOCK_SIZE;
    void* volatile retval = nullptr;
    __asm__ volatile ("mv a1, %[size]\n\t"
                      "li a0, %[ecallNum]\n\t"
                      "ecall"
                      : :
                      [size] "r" (size),
                      [ecallNum] "i" (Riscv::MEM_ALLOC)
                      );
    __asm__ volatile ("mv %0, a0" : "=r" (retval));
    return retval;
}

int mem_free(void *addr) {
    int retval;
    __asm__ volatile ("mv a1, %0\n\t"
                      "li a0, %[ecallNum]\n\t"
                      "ecall"
                      : :
                      "r" (addr),
                      [ecallNum] "i" (Riscv::MEM_FREE)
                      );
    __asm__ volatile ("mv %0, a0" : "=r" (retval));
    return retval;
}

size_t mem_get_free_space() {
    size_t retval;
    __asm__ volatile ("li a0, %[ecallNum]\n\t"
                      "ecall"
                      : :
                      [ecallNum] "i" (Riscv::MEM_GET_FREE_SPACE)
                      );
    __asm__ volatile ("mv %0, a0" : "=r" (retval));
    return retval;
}

size_t mem_get_largest_free_block() {
    size_t retval;
    __asm__ volatile ("li a0, %[ecallNum]\n\t"
                      "ecall"
                      : :
                      [ecallNum] "i" (Riscv::MEM_GET_LARGEST_FREE_BLOCK)
                      );
    __asm__ volatile ("mv %0, a0" : "=r" (retval));
    return retval;
}

int thread_create(thread_t *handle, void (*start_routine)(void *), void *arg) {
    void volatile *stack_space = mem_alloc(DEFAULT_STACK_SIZE);
    int volatile retval;
    __asm__ volatile ("mv a4, %0\n\t"
                      "mv a3, %1\n\t"
                      "mv a2, %2\n\t"
                      "mv a1, %3\n\t"
                      "li a0, %[ecallNum]\n\t"
                      "ecall"
                      : :
                      "r" (stack_space),
                      "r" (arg),
                      "r" (start_routine),
                      "r" (handle),
                      [ecallNum] "i" (Riscv::THREAD_CREATE)
                      );
    __asm__ volatile ("mv %0, a0" : "=r" (retval));
    return retval;
}

int thread_exit() {
    int volatile retval;
    __asm__ volatile ("li a0, %[ecallNum]\n\t"
                      "ecall"
                      : :
                      [ecallNum] "i" (Riscv::THREAD_EXIT)
                      );
    __asm__ volatile ("mv %0, a0" : "=r" (retval));
    return retval;
}

void thread_dispatch() {
    __asm__ volatile ("li a0, %[ecallNum]\n\t"
                      "ecall"
                      : :
                      [ecallNum] "i" (Riscv::THREAD_DISPATCH)
                      );
}

int sem_open(sem_t *handle, unsigned int init) {
    int volatile retval;
    __asm__ volatile ("mv a2, %[init]\n\t"
                      "mv a1, %[handle]\n\t"
                      "li a0, %[ecallNum]\n\t"
                      "ecall"
                      : :
                      [init] "r" (init),
                      [handle] "r" (handle),
                      [ecallNum] "i" (Riscv::SEM_OPEN)
                      );
    __asm__ volatile ("mv %0, a0" : "=r" (retval));
    return retval;
}

int sem_close(sem_t handle) {
    int retval;
    __asm__ volatile (
                      "mv a1, %[handle]\n\t"
                      "li a0, %[ecallNum]\n\t"
                      "ecall"
                      : :
                      [handle] "r" (handle),
                      [ecallNum] "i" (Riscv::SEM_CLOSE)
                      );
    __asm__ volatile ("mv %0, a0" : "=r" (retval));
    return retval;
}

int sem_wait(sem_t id) {
    int retval;
    __asm__ volatile ("mv a1, %[id]\n\t"
                      "li a0, %[ecallNum]\n\t"
                      "ecall"
                      : :
                      [id] "r" (id),
                      [ecallNum] "i" (Riscv::SEM_WAIT)
                      );
    __asm__ volatile ("mv %0, a0" : "=r" (retval));
    return retval;
}

int sem_signal(sem_t id) {
    int retval;
    __asm__ volatile ("mv a1, %[id]\n\t"
                      "li a0, %[ecallNum]\n\t"
                      "ecall"
                      : :
                      [id] "r" (id),
                      [ecallNum] "i" (Riscv::SEM_SIGNAL)
                      );
    __asm__ volatile ("mv %0, a0" : "=r" (retval));
    return retval;
}

int time_sleep(time_t time) {
    int retval;
    __asm__ volatile ("mv a1, %[time]\n\t"
                      "li a0, %[ecallNum]\n\t"
                      "ecall"
                      : :
                      [time] "r" (time),
                      [ecallNum] "i" (Riscv::TIME_SLEEP)
                      );
    __asm__ volatile ("mv %0, a0" : "=r" (retval));
    return retval;
}

char getc() {
    char volatile retval;
    __asm__ volatile ("li a0, %[ecallNum]\n\t"
                      "ecall"
                      : :
                      [ecallNum] "i" (Riscv::GETC)
                      );
    __asm__ volatile ("mv %0, a0" : "=r" (retval));

    //echo
    //putc(retval);

    return retval;
}

void putc(char character) {
    __asm__ volatile ("mv a1, %[character]\n\t"
                      "li a0, %[ecallNum]\n\t"
                      "ecall"
                      : :
                      [character] "r" (character),
                      [ecallNum] "i" (Riscv::PUTC)
                      );
}

