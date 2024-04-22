BITS 64
       
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
        mov rax, 1
        mov rdi, 1
        lea rsi, [rel msg]
        mov rdx, 8
        syscall

        ; load context
        pop r11
        pop rdi
        pop rsi
        pop rdx
        pop rcx
        pop rax

        ; Jump without looking back
        mov rax, 0x4022e0
        jmp rax

msg db "Hi mom!", 0xa , 0
