section .data
    header db "Content-Type: text/html", 10, 10
    header_len equ $ - header

    body db "<html><body><h1>Hello from Assembly CGI!</h1></body></html>", 10
    body_len equ $ - body

section .text
    global _start

_start:
    ; Write header to stdout (fd = 1)
    mov rax, 1          ; sys_write
    mov rdi, 1          ; fd = stdout
    mov rsi, header
    mov rdx, header_len
    syscall

    ; Write body to stdout
    mov rax, 1
    mov rdi, 1
    mov rsi, body
    mov rdx, body_len
    syscall

    ; Exit
    mov rax, 60         ; sys_exit
    xor rdi, rdi        ; status = 0
    syscall