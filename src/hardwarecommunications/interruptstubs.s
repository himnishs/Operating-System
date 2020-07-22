.set IRQ_BASE, 0x20

.section .text

.extern _ZN4myos21hardwarecommunication16InterruptManager15HandleInterruptEhj

.macro HandleException num
.global _ZN4myos21hardwarecommunication16InterruptManager15HandleInterruptEhj\num\()Ev
_ZN4myos21hardwarecommunication16InterruptManager15HandleInterruptEhj\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN4myos21hardwarecommunication16InterruptManager15HandleInterruptEhj\num\()Ev
_ZN4myos21hardwarecommunication16InterruptManager15HandleInterruptEhj\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    jmp int_bottom
.endm

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01
HandleInterruptRequest 0x0C

int_bottom:

    # save registers
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs
    
    pushl %ebp
    pushl %edi
    pushl %esi

    # call C++ Handler
    pushl %esp
    push (interruptnumber)
    call _ZN16InterruptManager15handleInterruptEhj
    #add %esp, 6
    mov %eax, %esp # switch the stack

    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa
    
.global _ZN4myos21hardwarecommunication16InterruptManager15HandleInterruptEhj
_ZN4myos21hardwarecommunication16InterruptManager15HandleInterruptEhj:
    
    iret

.data
    interruptnumber: .byte 0