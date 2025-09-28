.data
.intel_syntax noprefix
.global _start
.text
_start:
    mov rax, 5
    push rax

    mov rax, qword ptr[rsp]
    push rax

    mov rax, 1
    push rax

    pop rbx
    pop rax
    add rax, rbx
    push rax

    mov rax, qword ptr[rsp]
    push rax

    pop rbx
    pop rax
    add rax, rbx
    push rax

    mov rax, 60
    pop rdi
    syscall

