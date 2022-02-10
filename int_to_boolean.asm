int_to_boolean: ; compiler generated, just generalized
mov eax, [esp + 4]
neg eax
sbb eax, eax
inc eax 
retn