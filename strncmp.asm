;compiler generated code
;just generalized it 
strncmp:
mov ecx, [ebp+0Ch]
jecxz epilog
mov ebx, ecx
mov edi, [ebp+4]
mov esi, edi
xor eax, eax
repne scasb
neg ecx
add ecx, ebx
mov edi, esi
mov esi, [ebp+8]
repe cmpsb
mov al, [esi-1]
xor ecx, ecx
cmp al, [edi-1]
ja above
jz epilog 
dec ecx 
dec ecx 
above:
not ecx
epilog:
mov eax, ecx
retn 
