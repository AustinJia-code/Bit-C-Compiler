.intel_syntax noprefix
.global main

main:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    mov ebx, 14
    mov eax, ebx
    jmp .Lfunc_2
.Lfunc_2:
    lea rsp, [rbp - 24]
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret