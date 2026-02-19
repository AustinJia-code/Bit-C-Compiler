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
    mov ecx, r13d
    mov eax, r12d
    imul eax, ecx
    mov r12d, eax
    mov ecx, r12d
    mov eax, ebx
    add eax, ecx
    mov ebx, eax
    mov eax, ebx
    jmp .Lfunc_2
.Lfunc_2:
    lea rsp, [rbp - 24]
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret