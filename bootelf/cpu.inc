
		CPU_8086	equ	0			; 8086/8088
		CPU_80186	equ	1			; 80186
		CPU_80286	equ	2			; 80286
		CPU_80386	equ	3			; 80386
		CPU_80486	equ	4			; early 80486
		CPU_CPUID	equ	5			; late 486 or 586
        CPU_MAX     equ CPU_CPUID
		
	_is8086 :	
		push	sp
		pop		ax
		sub		ax,		sp		
		ret
				
    _is80186 :
		pushf				
		pushf
		pop		ax				
		and		ax,		0x0fff				
		push	ax
		popf				
		pushf
		pop		ax				
		and		ax,		0xf000
		cmp		ax,		0xf000
		mov		ax,		-1
		jz		_is80186@1
		xor		ax,		ax
	_is80186@1 :
		popf
		ret
	
	_is80286 :
		pushf
		pushf
		pop		ax 
		or		ax,		0xf000
		push	ax
		popf
		pushf
		pop		ax
		and		ax,		0xf000
		or		ax,		ax	
		mov		ax,		-1
		jz		_is80286@1
		xor		ax,		ax
	_is80286@1 :
		popf
		ret

	_is80386 :		
		pushf
		push	bx		
		pushf	
		pop		ax
		mov		bx,		ax
		xor		ax,		0x4000
		push	ax
		popf	
		pushf
		pop		ax
		sub		ax,		bx
		jz		_is80386@1
		xor		ax,		ax
		jmp		_is80386@2		
	_is80386@1 :		
		mov		ax,		-1
	_is80386@2 :	
		pop		bx
		popf
		ret
		
	_is80486 :
		pushfd
		push	ebx		
		pushfd
		pop		eax
		xor		eax,	0x40000
		mov		ebx,	eax
		push	eax
		popfd
		pushfd
		pop		eax
		cmp		eax,	ebx
		mov		eax,	-1
		jnz		_is486@1
		xor		eax,	eax		
	_is486@1 :		
		pop		ebx
		popfd
		ret

	_hasCPUID :
		pushfd
		push	ebx		
		pushfd
		pop		eax
		xor		eax,	0x200000
		mov		ebx,	eax
		push	eax
		popfd
		pushfd
		pop		eax
		cmp		eax,	ebx
		mov		eax,	-1
		jnz		_hasCPUID@1
		xor		eax,	eax		
	_hasCPUID@1 :		
		pop		ebx
		popfd
		ret
		
	_cpu_get_model :
		push	bx	
		
		mov		bx,		CPU_8086
		call	_is8086
		or		ax,		ax
		jnz		_cpu_get_model@1 
		
		mov		bx,		CPU_80186
		call	_is80186
		or		ax,		ax
		jnz		_cpu_get_model@1 
		
		mov		bx,		CPU_80286
		call	_is80286
		or		ax,		ax
		jnz		_cpu_get_model@1 
		
		mov		bx,		CPU_80386
		call	_is80386
		or		ax,		ax
		jnz		_cpu_get_model@1 
		
		mov		bx,		CPU_80486
		call	_is80486
		or		ax,		ax
		jnz		_cpu_get_model@1 
		call	_hasCPUID
		or		ax,		ax
		jnz		_cpu_get_model@1 
		mov		bx,		CPU_CPUID				
	_cpu_get_model@1 :
		mov		ax,		bx
		pop		bx
		ret

;    _cpu_get_model_str :
;        push    bp
;        mov     bp,     sp
;        push    bx
;        mov     bx,     [bp+4]
;        cmp     bx,     CPU_MAX
;        jbe     _cpu_get_model_str@1 
;        mov     bx,     CPU_MAX
;    _cpu_get_model_str@1 :
;        shl     bx,     1
;        mov     ax,     [_cpu_strings+bx]
;        pop     bx
;        pop     bp
;        ret
		
		
;    _cpu_strings_0 :
;        db      "8086",0
;    _cpu_strings_1 :        
;        db      "80186",0
;    _cpu_strings_2 :        
;        db      "80286",0
;    _cpu_strings_3 :        
;        db      "80386",0
;    _cpu_strings_4 :        
;        db      "80486",0
;    _cpu_strings_5 :        
;        db      "80486+ with CPUID",0
;    _cpu_strings :
;        dw      _cpu_strings_0
;        dw      _cpu_strings_1
;        dw      _cpu_strings_2
;        dw      _cpu_strings_3
;        dw      _cpu_strings_4
;        dw      _cpu_strings_5        
        

		
		
