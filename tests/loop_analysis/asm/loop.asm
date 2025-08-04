global MOVAllBytesASM
global NOPAllBytesASM
global CMPAllBytesASM
global CMPAllBytesASMMisaligned
global DECAllBytesASM

section .text

;first paramter -> rdi
;second parameter -> rsi

MOVAllBytesASM:
    xor rax, rax
.loop:
    mov [rsi + rax], al
    inc rax
    cmp rax, rdi
    jb .loop
    ret


NOPAllBytesASM:
    xor rax, rax
.loop:
    nop
    add rax, 1
    cmp rax, rdi
    jb .loop
    ret

CMPAllBytesASM:
    xor rax, rax
.loop:
    inc rax
    cmp rax, rdi
    jb .loop
    ret

CMPAllBytesASMMisaligned:
    xor rax, rax
    align 64

    %rep 62
    nop
    %endrep
.loop:
    inc rax
    cmp rax, rdi
    jb .loop
    ret

DECAllBytesASM:
.loop:
    dec rdi
    jnz .loop
    ret

