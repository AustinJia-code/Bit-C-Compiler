.intel_syntax noprefix
.global main

double_it:
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov DWORD PTR [rbp +-8], edi
    mov eax, DWORD PTR [rbp +-8]
    push rax
    mov eax, DWORD PTR [rbp +-8]
    push rax
    pop rbx
    pop rax
    add eax, ebx
    push rax
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret
inc:
    push rbp
    mov rbp, rsp
    sub rsp, 8
    mov DWORD PTR [rbp +-8], edi
    mov eax, DWORD PTR [rbp +-8]
    push rax
    push 1
    pop rbx
    pop rax
    add eax, ebx
    push rax
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret
main:
    push rbp
    mov rbp, rsp
    push 20
    pop rdi
    call inc
    push rax
    pop rdi
    call double_it
    push rax
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret