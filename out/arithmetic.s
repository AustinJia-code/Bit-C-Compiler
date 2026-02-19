.intel_syntax noprefix
.global main

main:
    push rbp
    mov rbp, rsp
    push 2
    push 3
    push 4
    pop rbx
    pop rax
    imul eax, ebx
    push rax
    pop rbx
    pop rax
    add eax, ebx
    push rax
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret