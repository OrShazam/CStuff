;compiler generated code
;just generalized it 
strrchr:
mov edi, [ebp+8]
xor eax, eax
or ecx, 0FFFFFFFFh
repne scasb
inc ecx 
neg ecx
dec edi
mov al, [ebp+4]
std 
repne scasb
inc edi 
cmp [edi], al 
jz success
xor eax, eax
jmp end
success:
mov eax, edi
end:
cld
retn