.data
.intel_syntax noprefix
.global _start
.text
_start:
    mov rax, 5
    push rax

    mov rax, 2
    cmp qword ptr[rsp], rax
    ja _if_0_success

    mov rax, 2
    push rax

    mov rax, qword ptr[rsp]
    push rax

    mov rax, 60
    pop rdi
    syscall

_if_0_success:
    mov rax, 60
    pop rdi
    syscall

