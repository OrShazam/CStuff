; compiler generated - just generalized
memset:
mov edx, [esp + 0Ch]
mov ecx, [esp + 4]
test edx, edx
jz epilog
xor eax, eax
mov al, [esp + 8]
mov edi, ecx 
cmp edx, 4
jb set_byte_loop2
neg ecx
and ecx, 3
jz set_dword_loop
sub edx, ecx

set_byte_loop1:
mov [edi], al 
inc edi
loop set_byte_loop1

set_dword_loop:
mov ecx, eax
shl eax, 8
add eax, ecx
mov ecx, eax
shl eax, 10h
add eax, ecx 
mov ecx, edx
shr ecx, 2
jz epilog 
rep stosd
jmp epilog

set_byte_loop2:
mov [edi], al
inc edi
loop set_byte_loop_2

epilog:
mov eax, [esp + 4]
retn