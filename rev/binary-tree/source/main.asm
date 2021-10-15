global _start

section .text
f:
    lea rax, [rel mod]
    mov r8, [rbx]
    xor [rax], r8
    mov r8, [rbx + 8]
    xor [rax + 8], r8
    mov r8, [rbx + 16]
    xor [rax + 16], r8
    mov r8, [rbx + 24]
    xor [rax + 24], r8
    mov al, [rsi]
    add rsi, 1
    test al, al
mod:
    incbin 'last.bin'

_start:
    xor rax, rax
    mov rdi, 0
    mov rdx, 101
    mov rsi, buffer
    syscall

    mov r10, 0
loop1:
    mov al, [r10+buffer]
    mov r11, 0
loop2:
    lea rcx, [8*r10+buffer_bin]
    add rcx, r11
    mov bl, al
    and bl, 1
    mov [rcx], bl
    sar al, 1
    inc r11
    cmp r11, 8
    jl loop2

    inc r10
    cmp r10, 100
    jl loop1

    mov rbx, xor_data
    mov rdi, xor_data
    mov rsi, buffer_bin
    mov r10, pass
    mov r11, fail
    call f

section .data
pass: db "PASS", 10
fail: db "FAIL", 10
xor_data: incbin 'code.bin'

section .bss
buffer: resb 101
buffer_bin: resb 808

