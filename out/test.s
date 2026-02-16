.intel_syntax noprefix
.global main

main:
    push rbp
    mov rbp, rsp
    sub rsp, 8
    push 0
    pop rax
    mov DWORD PTR [rbp +-8], eax
    sub rsp, 8
    push 0
    pop rax
    mov DWORD PTR [rbp +-16], eax
.L2:
    mov eax, DWORD PTR [rbp +-8]
    push rax
    push 5
    pop rbx
    pop rax
    cmp eax, ebx
    setl al
    movzx eax, al
    push rax
    pop rax
    test eax, eax
    je .L3
    mov eax, DWORD PTR [rbp +-16]
    push rax
    mov eax, DWORD PTR [rbp +-8]
    push rax
    pop rbx
    pop rax
    add eax, ebx
    push rax
    pop rax
    mov DWORD PTR [rbp +-16], eax
    mov eax, DWORD PTR [rbp +-8]
    push rax
    push 1
    pop rbx
    pop rax
    add eax, ebx
    push rax
    pop rax
    mov DWORD PTR [rbp +-8], eax
    jmp .L2
.L3:
    mov eax, DWORD PTR [rbp +-16]
    push rax
    pop rax
    mov rsp, rbp
    pop rbp
    ret
    mov rsp, rbp
    pop rbp
    ret