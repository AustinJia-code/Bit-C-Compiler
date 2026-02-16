.intel_syntax noprefix
.global main

main:
    push rbp
    mov rbp, rsp
    push 2
    push 3
    push 4
    pop ebx
    pop eax
    imul eax, ebx
    push eax
    pop ebx
    pop eax
    add eax, ebx
    push eax
    pop eax
    mov rsp, rbp
    pop rbp
    ret