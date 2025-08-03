global read_1
global read_2
global read_3
global read_4

global write_1
global write_2
global write_3

section .text

;first paramter -> rdi
;second parameter -> rsi

read_1_4x2:
    align 64
    xor rax, rax
.loop:
    mov al, [rsi]
    mov al, [rsi]
    sub rdi, 2
    jnle .loop
    ret

read_1x2:
    align 64
    xor rax, rax
.loop:
    mov al, [rsi]
    mov al, [rsi]
    sub rdi, 2
    jnle .loop
    ret

read_8x2:
    align 64
    xor rax, rax
.loop:
    mov rax, [rsi]
    mov rax, [rsi]
    sub rdi, 2
    jnle .loop
    ret


read_1:
    align 64
    xor rax, rax
.loop:
    mov rax, [rsi]
    sub rdi, 1
    jnle .loop
    ret

read_2:
    align 64
    xor rax, rax
.loop:
    mov rax, [rsi]
    mov rax, [rsi]
    sub rdi, 2
    jnle .loop
    ret

read_3:
    align 64
    xor rax, rax
.loop:
    mov rax, [rsi]
    mov rax, [rsi]
    mov rax, [rsi]
    sub rdi, 3
    jnle .loop
    ret

read_4:
    align 64
    xor rax, rax
.loop:
    mov rax, [rsi]
    mov rax, [rsi]
    mov rax, [rsi]
    mov rax, [rsi]
    sub rdi, 4
    jnle .loop
    ret

write_1:
    align 64
    xor rax, rax
.loop:
    mov [rsi], rax
    sub rdi, 1
    jnle .loop
    ret

write_2:
    align 64
    xor rax, rax
.loop:
    mov [rsi], rax
    mov [rsi], rax
    sub rdi, 2
    jnle .loop
    ret

write_3:
    align 64
    xor rax, rax
.loop:
    mov [rsi], rax
    mov [rsi], rax
    mov [rsi], rax
    sub rdi, 3
    jnle .loop
    ret
