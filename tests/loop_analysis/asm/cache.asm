global test_L1
global test_L2
global test_L3
global test_ram

section .text

;first paramter -> rdi
;second parameter -> rsi

test_L1:
    align 64
    xor rax, rax
    xor r8, r8
    xor r9, r9
    mov r9, rsi
.loop:
    vmovdqu ymm0, [r9]
    vmovdqu ymm1, [r9 + 32]
    vmovdqu ymm2, [r9 + 64]
    vmovdqu ymm3, [r9 + 96]

    add rax, 128
    add r8, 128
    and r8, 0x7FFF

    mov r9, rsi
    add r9, r8

    cmp rax, rdi
    jb .loop
    ret

test_L2:
    align 64
    xor rax, rax
    xor r8, r8
    xor r9, r9
    mov r9, rsi
.loop:
    vmovdqu ymm0, [r9]
    vmovdqu ymm1, [r9 + 32]
    vmovdqu ymm2, [r9 + 64]
    vmovdqu ymm3, [r9 + 96]

    add rax, 128
    add r8, 128
    and r8, 0xFFFF

    mov r9, rsi
    add r9, r8

    cmp rax, rdi
    jb .loop
    ret

test_L3:
    align 64
    xor rax, rax
    xor r8, r8
    xor r9, r9
    mov r9, rsi
.loop:
    vmovdqu ymm0, [r9]
    vmovdqu ymm1, [r9 + 32]
    vmovdqu ymm2, [r9 + 64]
    vmovdqu ymm3, [r9 + 96]

    add rax, 128
    add r8, 128
    and r8, 0x1FFFFF

    mov r9, rsi
    add r9, r8

    cmp rax, rdi
    jb .loop
    ret

test_ram:
    align 64
    xor rax, rax
.loop:
    vmovdqu ymm0, [rsi]
    vmovdqu ymm1, [rsi + 32]
    vmovdqu ymm2, [rsi + 64]
    vmovdqu ymm3, [rsi + 96]

    add rsi, 128
    add rax, 128

    cmp rax, rdi
    jb .loop
    ret
