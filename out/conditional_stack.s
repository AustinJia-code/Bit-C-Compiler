.intel_syntax noprefix
.global main

main:
    push rbp
    mov rbp, rsp
    push 1
    push 5
    pop rbx
    pop rax
    cmp eax, ebx
    setl al
    movz eax, al
    push rax
    pop rax
    test eax, eax
    je .L2
    push 42
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    jmp .L3
.L2:
.L3:
    push 13
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret