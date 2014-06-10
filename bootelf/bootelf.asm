        format  binary

        org     0x8000

        jmp     _start 

        include "cpu.inc"
        include "bios.inc"
        include "a20.inc"

        MIN_MEM equ 4096
        MIN_CPU equ CPU_80386
		
		_param_cpu			equ 0x7dfc
		_param_mem 			equ 0x7e00
		_param_mmap_size 	equ 0x7e04
		_param_mmap 		equ 0x7e08

    _start :

        cli 
        mov     ax,     cs
        mov     ds,     ax
        mov     sp,     ax
        mov     ax,     0x9000
        mov     ss,     ax
		xor		ax,		ax
		mov		es,		ax
		
        sti

        call    _cpu_get_model
        cmp     ax,     MIN_CPU
        jnl     .cpu_ok

        mov     ax,     _error_cpu
        push    ax
        call    _puts     
        add     sp,     2

        mov     ax,     _error_press
        push    ax
        call    _puts
        add     sp,     2

        sti
        xor     ax,     ax
        int     0x16
        int     0x19

    .cpu_ok :
		and		eax,	0xffff
        mov     [es:_param_cpu],eax
		
		xor		eax,	eax
		xor		edx,	edx
		
        call    _memory_get_size
        shl		edx,	16
		add		eax,	edx
		        		
        cmp     eax,     MIN_MEM
        jnl     .mem_ok
	
        mov     ax,     _error_mem
        push    ax
        call    _puts     
        add     sp,     2
		
	.die:
        mov     ax,     _error_press
        push    ax
        call    _puts
        add     sp,     2

        sti
        xor     ax,     ax
        int     0x16
        int     0x19
        
    .mem_ok :        
		
        mov     [es:_param_mem],eax        
				
		mov		di,		_param_mmap
				
		call	_memory_map_e820
		mov		[es:_param_mmap_size],eax
				
        call    _flat_a20_on
		push	dword [es:0]
		mov		ax,		0xffff
		mov		es,		ax
		mov		[es:0x10],dword 0xDEADBEEF
		cmp		dword [es:0],0xDEADBEEF
		pop		dword [es:0]		
				
		jnz		_switch
		
        mov     ax,     _error_press
        push    ax
        call    _puts
		add     sp,     2
		
		jmp		.die
		
    _error_cpu :
        db      "This system fails to meet the cpu requiroment.",10
        db      "A 386 or better is needed to run this software.",10,0
    _error_mem :
        db      "This system fails to meet the memory requiroments.",10
        db      "4MB or more memory is needed to run this software.",10,0
    _error_press :
        db      "Press any key to reboot...",10,0 
    _error_a20 :
		db		"A20 ERROR!",10,0
	

;   =================================================
    _gdt :
        dq      0
        dq      0x00DF9A000000FFFF         
        dq      0x00DF93000000FFFF
        dq      0x00009A000000FFFF         
    _gdtr :
        dw      _gdtr-_gdt
        dd      _gdt

;   =================================================

;   =================================================
        
    _switch :
        cli
        lgdt    [_gdtr]
        smsw    ax
        xor     ax,     1
        lmsw    ax
        jmp     0x8:$+5

        use32

        mov     eax,    0x10
        mov     ds,     ax
        mov     es,     ax
        mov     fs,     ax
        mov     gs,     ax
        mov     ss,     ax
        mov     esp,    0xa0000

        EI_MAGIC        equ dword ptr ehdr + 0
        EI_CLASS        equ byte ptr ehdr + 4   
        EI_DATA         equ byte ptr ehdr + 5
        E_TYPE          equ word ptr ehdr + 16
        E_MACHINE       equ word ptr ehdr + 18
        E_ENTRY         equ dword ptr ehdr + 24
        E_PHOFF         equ dword ptr ehdr + 28
        E_PHENTSIZE     equ word ptr ehdr + 42
        E_PHNUM         equ word ptr ehdr + 44

        P_TYPE          equ 0
        P_OFFSET        equ 4
        P_VADDR         equ 8
        P_PADDR         equ 12
        P_FILESZ        equ 16
        P_MEMSZ         equ 20
        P_FLAGS         equ 24
        P_ALIGN         equ 28


        cmp     EI_MAGIC,   0x464c457f      ; check ELF signature 
        jnz     lockup 
        cmp     EI_CLASS,   1               ; check if 32bit image
        jnz     lockup 
        cmp     EI_DATA,    1               ; check if little endian        
        jnz     lockup 
        cmp     E_MACHINE,  3               ; check if x86
        jnz     lockup 
        cmp     E_TYPE,     2               ; check if executable
        jnz     lockup
        
        mov     dx,        	E_PHNUM
        and     edx,        0xffff
        mov     ebx,        E_PHOFF
        add     ebx,        ehdr
        cld

    .lseg :

        mov     esi,        [ebx+P_OFFSET]
        add     esi,        ehdr
        mov     edi,        [ebx+P_VADDR]
        mov     ecx,        [ebx+P_FILESZ]
        shr     ecx,        1
        adc     ecx,        0
        shr     ecx,        1
        adc     ecx,        0
        rep     movsd

        mov     ecx,        [ebx+P_MEMSZ]
        sub     ecx,        [ebx+P_FILESZ]       
        xor     eax,        eax
        shr     ecx,        1
        adc     ecx,        0
        shr     ecx,        1
        adc     ecx,        0
        rep     stosd

        mov     ax,         E_PHENTSIZE
        and     eax,        0xffff
        add     ebx,        eax
        
        dec     edx
        jnz     .lseg

        call    E_ENTRY
        
    lockup :
        cli
        hlt

        times 512 - ($ mod 512) db 0
    ehdr :        
        
        
