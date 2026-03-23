# 1 "src/trap.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/riscv64-linux-gnu/include/stdc-predef.h" 1 3
# 32 "<command-line>" 2
# 1 "src/trap.S"
.extern _ZN5Riscv11trapHandlerEv
.extern _ZN3TCB7runningE

.align 4
.global _ZN5Riscv4trapEv
.type _ZN5Riscv4trapEv @function
_ZN5Riscv4trapEv:

    csrw sscratch, a0
    ld a0, _ZN3TCB7runningE
    sd sp, 1 * 8(a0)
    ld sp, 2 * 8(a0)
    csrr a0, sscratch

    addi sp, sp, -256
    .irp index 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8 (sp)
    .endr

    call _ZN5Riscv11trapHandlerEv

    .irp index 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8 (sp)
    .endr
    addi sp, sp, 256

    csrw sscratch, a0
    ld a0, _ZN3TCB7runningE
    sd sp, 2 * 8(a0)
    ld sp, 1 * 8(a0)
    csrr a0, sscratch

    sret
