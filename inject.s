BITS 64

section .data
msg db "Hi mom!", 0xA ; The string to print, followed by a newline character
len equ $ - msg ; The length of the string

section .text
global main

main:
        ; save context
        push rax
        push rcx
        push rdx
        push rsi
        push rdi
        push r11

        ; write "Hi mom!" to stdout
        mov rax, 1 ; syscall number (sys_write)
        mov rdi, 1 ; file descriptor (stdout)
        mov rsi, msg ; address of the string
        mov rdx, len ; length of the string
        syscall ; invoke the system call

        ; exit
        mov rax, 60 ; syscall number (sys_exit)
        xor rdi, rdi ; exit code (0)
        syscall ; invoke the system call

        ; load context
        pop r11
        pop rdi
        pop rsi
        pop rdx
        pop rcx
        pop rax

        ; return
        ret
