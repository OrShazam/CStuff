;gcc generated, not my own code
;edx -> edx % 10
        mov     ecx, edx
        movsx   rax, ecx
        imul    rax, rax, 1717986919
        shr     rax, 32
        sar     eax, 2
        mov     esi, ecx
        sar     esi, 31
        sub     eax, esi
        mov     edx, eax
        mov     eax, edx
        sal     eax, 2
        add     eax, edx
        add     eax, eax
        sub     ecx, eax
        mov     edx, ecx
;edx -> edx / 10
	mov     eax, edx
        movsx   rdx, eax
        imul    rdx, rdx, 1717986919
        shr     rdx, 32
        sar     edx, 2
        sar     eax, 31
        mov     ecx, eax
        sub 	edx, ecx

