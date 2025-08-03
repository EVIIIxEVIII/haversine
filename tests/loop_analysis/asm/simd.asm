global read_4x2
global read_8x2
global read_16x2
global read_32x2

section .text

read_4x2:
    align 64
    xor rax, rax
.loop:
    mov r8d, [rsi]
    mov r8d, [rsi + 4]
    add rax, 8
    cmp rax, rdi
    jb .loop
    ret

read_8x2:
    align 64
    xor rax, rax
    .loop:
    mov r8, [rsi]
    mov r8, [rsi + 8]
    add rax, 16
    cmp rax, rdi
    jb .loop
    ret

read_16x2:
    align 64
    xor rax, rax
.loop:
    vmovdqu xmm0, [rsi]
    vmovdqu xmm1, [rsi + 16]
    add rax, 32
    cmp rax, rdi
    jb .loop
    ret

read_32x2:
    align 64
    xor rax, rax
.loop:
    vmovdqu ymm0, [rsi]
    vmovdqu ymm1, [rsi + 32]
    add rax, 64
    cmp rax, rdi
    jb .loop
    ret
