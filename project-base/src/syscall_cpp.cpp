//
// Created by os on 11/25/25.
//
#include "../lib/mem.h"
#include "../h/syscall_c.hpp"
#include "../h/syscall_cpp.hpp"


using size_t = decltype(sizeof(0));

void *operator new(size_t n)
{
    return mem_alloc(n);
}

void *operator new[](size_t n)
{
    return mem_alloc(n);
}

//todo: vidi da li da vracas noexception specifier
void operator delete(void *p)
{
    mem_free(p);
}

void operator delete[](void *p)
{
    mem_free(p);
}

Thread::Thread(void (*body)(void *), void *arg) : body(body), arg(arg) {}

Thread::~Thread() {
    //thread_exit??
}

int Thread::start() {
    return thread_create(&myHandle, body, arg);
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t time) {
    return time_sleep(time);
}

Thread::Thread() : arg(this){
    body = [](void* arg){
        Thread *thread = (Thread *) arg;
        thread->run();
    };
}


Semaphore::Semaphore(unsigned int init) {
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore() {
    sem_close(myHandle);
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}


void PeriodicThread::terminate() {

}

PeriodicThread::PeriodicThread(time_t period) : period(period){}

void PeriodicThread::run() {
    periodicActivation();
    time_sleep(this->period);
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char character) {
    ::putc(character);
}
