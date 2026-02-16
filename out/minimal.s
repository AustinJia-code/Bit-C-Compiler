.intel_syntax noprefix
.global main

main:
    push rbp
    mov rbp, rsp
    mov eax, 42
    pop rbp
    ret