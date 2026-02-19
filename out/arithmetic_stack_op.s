.intel_syntax noprefix
.global main

main:
    push rbp
    mov rbp, rsp
    push 14
    pop rax
    mov rsp, rbp
    pop rbp
    ret