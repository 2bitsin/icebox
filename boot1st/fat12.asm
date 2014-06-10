
        LOADER_NAME         equ "BOOTI386SYS"

        FATLOADADDR         equ 0x8E800         ; adress where boot sector loads FAT..
        FATLOAD_SEGMENT     equ FATLOADADDR / 16
        FATLOAD_OFFSET      equ 0x0000

        SYSLOADADDR         equ 0x8000          ; adress where the loader is loaded in memory
        SYSLOAD_OFFSET      equ 0x0
        SYSLOAD_SEGMENT     equ SYSLOADADDR / 16 

        ENTRY_POINT_OFFSET  equ SYSLOADADDR
        ENTRY_POINT_SEGMENT equ 0

        FATNAMELEN          equ 11

        BytsPerSec          equ word ptr bpb + 11
        SecPerClus          equ byte ptr bpb + 13
        RsvdSecCnt          equ word ptr bpb + 14
        NumFATs             equ byte ptr bpb + 16
        RootEntCnt          equ word ptr bpb + 17
        FATSz16             equ word ptr bpb + 22
        SecPerTrk           equ word ptr bpb + 24
        NumHeads            equ word ptr bpb + 26
        DrvNum              equ byte ptr bpb + 36

        format  binary
        org     0x7c00

    bpb :

        jmp     near start
        
        db      0,0,0,0,0,0,0,0,0,0
        db      0,0,0,0,0,0,0,0,0,0
        db      0,0,0,0,0,0,0,0,0,0
        db      0,0,0,0,0,0,0,0,0,0
        db      0,0,0,0,0,0,0,0,0,0
        db      0,0,0,0,0,0,0,0,0


    start :
    
        xor     ax,     ax          ; ds = ss = 0
        mov     ds,     ax
        mov     ss,     ax
        mov     sp,     0x7c00      ; stack at 0:0x7C00
        mov     DrvNum, dl          ; save drive letter
        mov     ax,     0x800       ; get drive parameters ah = 0x8 
        int     0x13
        shr     dx,     8           ; get dh in to dl and zero out dh
        inc     dx                  ; increment it since it's 0 based
        mov     NumHeads,dx         ; save head number
        and     cx,     0x3F        ; get sector count (low 6 bits)
        mov     SecPerTrk,cx        ; save sector count

        mov     ax,     RsvdSecCnt  ; get the first sector of FAT
        mov     cx,     FATSz16     ; number of sectors per first fat
        mov     bx,     FATLOAD_SEGMENT ; Segment of fat load address
        mov     es,     bx         
        xor     bx,     bx          ; load fat at FATLOADADDR
        call    read                ; read the sectors
        add     ax,     cx          ; get sector of next fat
        add     ax,     FATSz16     ; ax = first sector of root directory
        mov     cx,     RootEntCnt  ; how many root entries ?
        shl     cx,     5           ; multiply by 32 (size of entry)
        add     cx,     0x1FF       ; add 511 so the sector count rounds down
        shr     cx,     9           ; divide 512
		mov	    bx,	    SYSLOAD_SEGMENT	; segment of load address
		mov	    es,	    bx
		xor	    bx,	    bx		    ; load the root directory to SYSLOADADDR
		call	read			    ; read sectors
		add	    ax,	    cx		    ; get the sector of first data cluster
		xor	    di,	    di		    ; es:di first dir entry
		mov	    bp,	    ax		    ; save it in bp

		mov     dx,     RootEntCnt  ; get number of root entries
    
    scan_dir :			        	; scan root directory

		mov	    si,	    load_name	; ds:si = offset of file name
		mov	    cx,	    FATNAMELEN	; length of file name (11 chars)
		push	di			
		repz	cmpsb		        ; scan file name
		pop	    di			 
		or	    cx,	    cx		    ; was it a perfect mach ? (cx = 0 ?)
		jz	    .done			    ; were done if so
		add	    di, 	0x20		; next 32bytes (next entry)
		dec	    dx		            ; have we reached the end ?
		jnz	    scan_dir		    ; if we didn't repeat
		jmp	    rd_error		    ; ENOENT

    .done :
		mov	    si,	    [es:di+26]	; get low word of first cluster
		mov	    bx,	    SYSLOAD_SEGMENT	
		mov	    es,	    bx		    ; es:bx = SYSLOADADDR

		load_loop : 			    ; file load loop
       
		cmp	    si,	    0xff8		; is it EOF ?
		jge	    sys_start		    ; if it was , we start it up

		mov	    ax,	    si		    ; ax = cluster to load
		sub	    ax,	    2		    ; (first 2 entries in FAT are unused) 
		xor	    dx,	    dx		    ; can't remember why i put this here O.o
		mov	    cl,	    SecPerClus	
		xor	    ch,	    ch		    ; cx = how much sector per one cluster
		mul	    cx			        ; ax = SectorPerCluster * ClusterToLoad (reletive offset)
		add	    ax,	    bp		    ; ax = first sector of cluster to load

		xor	    bx,	    bx		    ; load at es:0

		call	read			    ; read the sectors

		shl	    cx,	    5		    ; cx = (bytes loaded) / 16
		mov	    bx,	    es		
		add	    bx,	    cx		
		mov	    es,	    bx		    ; es += cx

		push	ds			        ; save DS
		mov	    bx,	    FATLOAD_SEGMENT	 
		mov	    ds,	    bx		    ; ds:0 = the FAT 

		mov	    bx,	    si		    ; bx = si
		shl	    bx,	    1		    ; bx = bx * 2
		add	    bx,	    si		    ; bx = si + bx
		shr	    bx, 	1		    ; bx = bx / 2
									; bx = (si * 3)/2
		mov	    bx,	    [bx]		; get word from FAT

		pop	    ds			        ; restore DS
    
		and	    si,	    1		    ; was it odd or even ?
		jz	    .even
		shr	    bx, 	4		
		jmp	    .done
	.even :
		and	    bx,	    0xfff
	.done :
		mov	    si,	    bx		    ; si = new cluster to load
		jmp	    load_loop			; repeat
	sys_start : 			    	; startup the loaded program
		xor	    ax,	    ax		
		mov	    ds,	    ax
		int	    0x13				; reset the floppy drive
		cli
		mov	byte [0x440],1			; disk motor time out
		int	0x8			        	; timer has gone off (turn off the motor)
		sti			       
		mov	dl,	DrvNum		     
		xor	dh,	dh		        	; dx = drive number

		jmp	ENTRY_POINT_SEGMENT:ENTRY_POINT_OFFSET	; jmp to loaded program

    read :				    		; read : ax = first sector , cx = count 
									; es:bx address to load to
		push	ax			    	; all registers preserved
		push	dx
		push	cx
		push	bx

		xor	    dx,	    dx			; zero out dx for divide
		div	    SecPerTrk			; get sector number
		inc	    dx			    	; increment sector number in dx (must be 1 based)
		mov	    bx,	    dx		

		xor	    dx, 	dx			; dx = 0 for divide
		div	    NumHeads			; get head number and cylinder
									; ax = cylinder , dx = head
		mov	    cx,	    ax			; look up an interrupt list 
		xchg	ch,	    cl			; for details here
		shl	    cl,	    6
		and	    bx,	    0x3F
		or	    cx,	    bx

		shl	    dx,	    8
		mov	    dl,	    DrvNum

		pop	    bx	 
		pop	    ax
		push	ax

		mov	    ah,	0x2

		int	    0x13

		pop	    cx
		pop	    dx
		pop	    ax

		jc	    rd_error	
		ret

    rd_error :              	; read error or file not found

		mov	    si,	    read_error
		call	print
		xor 	ax,	    ax
		int	    0x16
		int	    0x19
	
    print :			        	; print :  ds:si = null terminated string

		push	ax
		push	bx
		push	si
    .1 :
		lodsb
		or	    al,	    al
		jz	    .2
		xor	    bx,	    bx
		mov	    ah,	    0xE
		int	    0x10
		jmp	    .1
    .2 :
		pop	    si
		pop	    bx
		pop	    ax
		ret

    read_error :

		db	    "Read error or system files not present !",13,10
		db	    "Press a key to reboot ...",13,10,0

    load_name :

		db	    LOADER_NAME

	    
    times (510-$+$$) db 0

		dw	    0xaa55
