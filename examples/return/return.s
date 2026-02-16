.intel_syntax noprefix
.global main

main:
    push rbp
    mov rbp, rsp
    push 42
    pop eax
    mov rsp, rbp
    pop rbp
    ret