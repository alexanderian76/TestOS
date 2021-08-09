global long_mode_start

section .text
bits 64

extern main
extern isr1_handler
extern IDT


%macro PUSHALL 0
    push rax
    push rcx
    push rdx
    push r8
    push r9
    push r10
    push r11
%endmacro

%macro POPALL 0
    push r11
    push r10
    push r9
    push r8
    push rdx
    push rcx
    push rax
%endmacro
    


idtDescriptor:
    dw 4095
    dq IDT
isr1:
    PUSHALL
    call isr1_handler
    POPALL
    iretq
    GLOBAL isr1
LoadIDT:
    lidt[idtDescriptor]
    sti
    ret
    GLOBAL LoadIDT
    



long_mode_start:
    ; print `OKAY` to screen
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    
    mov rax, 0x2f592f412f4b2f4f
    mov qword [0xb8000], rax
    
   call main
   
    hlt
