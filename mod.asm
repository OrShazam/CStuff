;compiler generated code
;just generalized it 
;rdx sould be a power of 2
mod:
lea r8, [rdx-1]
mov rax, rcx
cqo
and rdx, r8
lea rax, [rax + rdx]
and rax, r8
sub rax, rdx
ret 