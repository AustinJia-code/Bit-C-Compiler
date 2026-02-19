.intel_syntax noprefix
.global main

double_it:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    sub rsp, 8
    mov DWORD PTR [rbp +-32], edi
    mov ebx, DWORD PTR [rbp +-32]
    mov r12d, DWORD PTR [rbp +-32]
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
inc:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    sub rsp, 8
    mov DWORD PTR [rbp +-32], edi
    mov ebx, DWORD PTR [rbp +-32]
    mov r12d, 1
    mov ecx, r12d
    mov eax, ebx
    add eax, ecx
    mov ebx, eax
    mov eax, ebx
    jmp .Lfunc_3
.Lfunc_3:
    lea rsp, [rbp - 24]
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
main:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    mov ebx, 20
    push rbx
    pop rdi
    call inc
    mov ebx, eax
    push rbx
    pop rdi
    call double_it
    mov ebx, eax
    mov eax, ebx
    jmp .Lfunc_4
.Lfunc_4:
    lea rsp, [rbp - 24]
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret