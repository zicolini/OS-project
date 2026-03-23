# 1 "src/threadInit.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/riscv64-linux-gnu/include/stdc-predef.h" 1 3
# 32 "<command-line>" 2
# 1 "src/threadInit.S"
.extern _ZN3TCB7runningE

.global _ZN5Riscv10threadInitEv
.type _ZN5Riscv10threadInitEv @function
_ZN5Riscv10threadInitEv:
csrw sepc, ra
csrw sscratch, a0
ld a0, _ZN3TCB7runningE
sd sp, 2 * 8(a0)
ld sp, 1 * 8(a0)
csrr a0, sscratch
sret
