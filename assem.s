.data
.intel_syntax noprefix
.global _start
.text
_start:
    mov rax, 10
    push rax

    mov rax, 1
    push rax

    mov rax, 1
    push rax

    pop rbx
    pop rax
    add rax, rbx
    push rax

    mov rax, 1
    push rax

    pop rbx
    pop rax
    add rax, rbx
    push rax

    mov rax, qword ptr[rsp+8]
    push rax

    pop rbx
    pop rax
    add rax, rbx
    push rax

    mov rax, 1
    push rax

    pop rbx
    pop rax
    add rax, rbx
    push rax

    mov rax, 60
    push QWORD ptr[rsp]
    pop rdi
    syscall
