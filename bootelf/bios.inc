;       ===================================================================
;       void __cdecl puts (char const *) ;
;       ===================================================================


    _puts :

        push    bp
        mov     bp,     sp

        push    ds
        push    si
        push    ax
        
        lds     si,     [bp+4]

    .1 :        

        lodsb
        or      al,     al
        jz      .2

        push    ax
        call    _putc
        add     sp,     2

        jmp     .1

    .2 :

        pop     ax
        pop     si
        pop     ds
        pop     bp
        ret

;       ===================================================================
;       void __cdecl putc (char) ;
;       ===================================================================
        
    _putc :

        push    bp
        mov     bp,     sp

        push    ax
        push    bx

        mov     ah,     0xe
        xor     bx,     bx

        mov     al,     [bp+4]
        cmp     al,     10

        jnz     .1

        push    ax
        mov     al,     13
        int     0x10
        pop     ax        

    .1 :
        int     0x10

        pop     bx
        pop     ax
        pop     bp

        ret

;       ===================================================================
;		u32_t _memory_size_e801 (void) ;
;       ===================================================================

	_memory_size_e801 :

		push	bx
		push	cx
		mov		ax,		0xe801
		int		0x15
        jc      .1
		mov 	dx,		bx
		mov		cx,		10
		shr		dx,		cl
		mov		cx,		6
		shl		bx,		cl
		add		ax,		bx
		adc		dx,		0				
        jmp     .2
    .1 :
		xor		dx,		dx
		xor		ax,		ax
    .2 :
		pop		cx
		pop		bx
		ret
		
;       ===================================================================
;		u32_t _memory_size_8800 (void) ;
;       ===================================================================

	_memory_size_8800 :
		
		mov		ax,		0x8800
		xor		dx,		dx
		int		0x15
        jnc     .1
		xor		ax,		ax
    .1 :
		ret
		
;       ===================================================================
;       u32_t _memory_get_size (void) ;
;       ===================================================================

    _memory_get_size :
        
        call    _memory_size_e801
        or      dx,     dx
        jnz     .nofb
        or      ax,     ax
        jnz     .nofb
        call    _memory_size_8800 
        xor     dx,     dx                
    .nofb :
        ret
		
;		===================================================================
;		u32_t _memory_map_e820 (smap_t * mmap) ;
;		===================================================================

		
	_memory_map_e820:
		push	ebx
		push	ecx		
		push	edx
		push	ebp
		xor		ebp,	ebp
		xor		ebx,	ebx
	.l1:
		mov		eax,	0xe820
		mov		edx,	'PAMS'
		mov		ecx,	24
		int		0x15
		jc		.done
		or		ebx,	ebx
		jz		.done
		cmp		eax,	'PAMS'
		jnz		.done
		cmp		ecx,	20
		jg		.l2
		mov		[es:di+20],dword 1
	.l2:
		jl		.l1
		add		di,		24
		inc		ebp
		cmp		ebp,	20
		ja		.done
		jmp		.l1
	.done:
		mov		eax,	ebp
		pop		ebp
		pop		edx
		pop		ecx
		pop		ebx
		ret
		
		
		
		