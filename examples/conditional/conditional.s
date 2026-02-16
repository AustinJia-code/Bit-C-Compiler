.intel_syntax noprefix
.global main

main:
    push rbp
    mov rbp, rsp
    push 1
    push 5
    pop ebx
    pop eax
    cmp eax, ebx
    setl al
    movzx eax, al
    push eax
    pop eax
    test eax, eax
    je .L2
    push 42
    pop eax
    jmp .L3
.L2:
.L3:
    push 13
    pop eax
    mov rsp, rbp
    pop rbp
    ret