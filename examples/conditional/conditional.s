.intel_syntax noprefix
.global main

main:
        push rbp
        mov rbp, rsp
        mov edx, 1
        mov eax, 5
        cmp edx, eax
        setl al
        test al, al
        je .L2
        mov eax, 42
        jmp .L3
.L2:
        mov eax, 13
.L3:
        pop rbp
        ret