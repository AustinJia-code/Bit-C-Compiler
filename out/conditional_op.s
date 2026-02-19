.intel_syntax noprefix
.global main

main:
    push rbp
    mov rbp, rsp
    push 42
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    push 13
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret