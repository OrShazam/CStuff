; compiler generated code, just generalized it 
strcat: 
mov edi, [esp+8]
or ecx, 0FFFFFFFFh
xor eax,eax 
repne scasb 
not ecx 
sub edi, ecx
mov esi, edi
mov edi, [esp+4]
mov ebx, ecx 
or ecx, 0FFFFFFFFh
repne scasb 
dec edi
mov ecx, ebx
shr ecx, 2
rep movsd
mov ecx, ebx
and ecx, 3
rep movsb
retn
