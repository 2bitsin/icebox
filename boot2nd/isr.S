        .text


        .global i386_isr_ptr
        .global i386_isr


    L_i386_isr :
        pushl   %esp
        addl    $8,     (%esp)
        pushl   %ebp
        pushl   %eax
        pushl   %ebx
        pushl   %ecx
        pushl   %edx
        pushl   %esi
        pushl   %edi
        pushl   %ds
        pushl   %es
        pushl   %fs
        pushl   %gs
        pushl   %ss
        movl    $0x10,  %eax
        movl    %eax,   %ds
        movl    %eax,   %es
        movl    %eax,   %fs
        movl    %eax,   %gs
        pushl   %esp        
        call    i386_isr
        addl    $24,    %esp        
        popl    %edi
        popl    %esi
        popl    %edx
        popl    %ecx
        popl    %ebx
        popl    %eax
        popl    %ebp
        popl    %esp
        iret
            
    i386_isr_ptr :        
        .long   L_i386_isr
