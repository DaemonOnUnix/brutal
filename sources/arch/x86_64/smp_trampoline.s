
[bits 16]

TRAMPOLINE_BASE equ 0x1000
SMP_INIT_PAGE_TABLE equ 0x500
SMP_INIT_STACK equ 0x570
SMP_INIT_GDT equ 0x580
SMP_INIT_IDT equ 0x590

; --- 16 BIT ---
extern smp_started_cpu_entry
global trampoline_start
trampoline_start:

    cli
    mov ax, 0x0
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    o32 lgdt [pm_gdtr - trampoline_start + TRAMPOLINE_BASE]

    mov eax, cr0
    or al, 0x1      ; enable protected mode
    mov cr0, eax

    jmp 0x8:(trampoline32 - trampoline_start + TRAMPOLINE_BASE)


[bits 32]


section .text

; ---- 32 BITS ----

trampoline32:

    mov bx, 0x10
    mov ds, bx
    mov es, bx
    mov ss, bx


    mov eax, dword [SMP_INIT_PAGE_TABLE]

    mov cr3, eax


    mov eax, cr4
    or eax, 1 << 5               ; Set the PAE-bit, which is the 6th bit (bit 5).
    or eax, 1 << 7
    mov cr4, eax


    mov ecx, 0xc0000080 ; efet register 
    rdmsr

    or eax,1 << 8  ; long mode enable
    wrmsr


    mov eax, cr0
    or eax, 1 << 31 ; paging 
    mov cr0, eax


    lgdt [lm_gdtr - trampoline_start + TRAMPOLINE_BASE]
    jmp 8:(trampoline64 - trampoline_start + TRAMPOLINE_BASE)



[bits 64]

; ---- 64 BITS ----

trampoline64:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov ax, 0x0
    mov fs, ax
    mov gs, ax


    lgdt [SMP_INIT_GDT]
    lidt [SMP_INIT_IDT]

    mov rsp, [SMP_INIT_STACK]

    mov rbp, 0x0 ; terminate stack traces here
    ; reset RFLAGS
    push 0x0
    popf


    mov rax, qword vcode64
    jmp rax


; virtual memory
vcode64:
    push rbp
    ; set up sse as higher half use it
    mov rax, cr0


    btr eax, 2 ; enable co processor + emulation
    bts eax, 1
    mov cr0, rax

    call smp_started_cpu_entry


; ------ DATA ------


; ---- Protected MODE ----

align 16
  pm_gdtr:
    dw pm_gdt_end - pm_gdt_start - 1
    dd pm_gdt_start - trampoline_start + TRAMPOLINE_BASE

align 16
  pm_gdt_start:
    ; null selector
    dq 0
    ; cs selector
    dq 0x00CF9A000000FFFF
    ; ds selector
    dq 0x00CF92000000FFFF
  pm_gdt_end:


; IDT
align 16
  pm_idtr:
    dw 0
    dd 0
    dd 0
    align 16

; ---- LONG MODE ----

align 16
  lm_gdtr:
    dw lm_gdt_end - lm_gdt_start - 1
    dq lm_gdt_start - trampoline_start + TRAMPOLINE_BASE

align 16
  lm_gdt_start:
    ; null selector
    dq 0
    ; 64bit cs selector
    dq 0x00AF98000000FFFF
    ; 64bit ds selector
    dq 0x00CF92000000FFFF
  lm_gdt_end:

global trampoline_end
trampoline_end: