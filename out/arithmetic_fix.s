.intel_syntax noprefix
.global main

main:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    mov ebx, 2
    mov r12d, 3
    mov r13d, 4
    imul r12d, r13d
    add ebx, r12d
    mov eax, ebx
    jmp .Lfunc_2
.Lfunc_2:
    lea rsp, [rbp - 24]
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret