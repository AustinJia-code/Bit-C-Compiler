.intel_syntax noprefix
.global main

main:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    mov ebx, 1
    mov r12d, 5
    cmp ebx, r12d
    setl bl
    movzx ebx, bl
    test ebx, ebx
    je .L3
    mov ebx, 42
    mov eax, ebx
    jmp .Lfunc_2
    jmp .L4
.L3:
.L4:
    mov ebx, 13
    mov eax, ebx
    jmp .Lfunc_2
.Lfunc_2:
    lea rsp, [rbp - 24]
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret