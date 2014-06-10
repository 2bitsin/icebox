
    _flat_a20_on:
        pushf  
		push	cx
        cli    
        xor     cx,cx
    .0 :
        in      al,64h
        test    al,02h
        jnz     .0
        mov     al,0d1h
        out     64h,al
        xor     cx,cx
    .1 :        
        in      al,64h
        test    al,02h
        jnz     .1
        mov     al,0dfh
        out     60h,al
        xor     cx,cx
    .2 :                
        in      al, 64h
        test    al, 02h
        jnz     .2 
		pop		cx
        popf    
        mov     ax, 1 		
        ret	
