global test_L1
global test_L2
global test_L3
global test_ram
global test_cache
global overflow_L1

section .text

;first paramter -> rdi
;second parameter -> rsi
;third parameter -> rdx

overflow_L1:
    align 64
    ; rdi - buffer count
    ; rsi - data ptr
    ; rdx - distance between addresses in bytes

    push r12
    push r13
    push r14
    xor r12, r12
    xor r13, r13
    xor r14, r14

    mov r12, 1000

.outer:
    mov r13, rsi
    mov r14, 256
    .inner:
        vmovdqu ymm0, [r13]
        vmovdqu ymm0, [r13 + 32]
        add r13, rdx
        dec r14
        jnz .inner

    dec r12
    jnz .outer

    pop r14
    pop r13
    pop r12
    ret


test_cache:
    ; rdi - buffer count
    ; rsi - data ptr
    ; rdx - chunk size

    align 64
    push r12
    push r13
    push r14
    push r15
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15; data pointer

    mov rax, rdx
    add rax, 128
    mov r12, rdx
    xor rdx, rdx
    mov r13, 256
    div r13
    mov r14, rax; inner loop cycles
    mul r13
    mov r13, rax

    mov rax, rdi
    div r13
    mov r12, rax; outer loop cycles
    mul r13

    xor r13, r13; inner loop counter

.outer:
    mov r13, r14
    mov r15, rsi
    .inner:
        vmovdqu ymm0, [r15]
        vmovdqu ymm1, [r15 + 32]
        vmovdqu ymm2, [r15 + 64]
        vmovdqu ymm3, [r15 + 96]
        vmovdqu ymm4, [r15 + 128]
        vmovdqu ymm5, [r15 + 160]
        vmovdqu ymm6, [r15 + 192]
        vmovdqu ymm7, [r15 + 224]

        add r15, 256
        dec r13
        jnz .inner

    dec r12
    jnz .outer

    pop r15
    pop r14
    pop r13
    pop r12
    ret


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
