.intel_syntax noprefix
.global main

main:
    push rbp
    mov rbp, rsp
    mov edx, 3
    mov eax, 4
    imul eax, edx
    mov edx, 2
    add eax, edx
    pop rbp
    ret
