while a < b do x = y

.L0:
mov eax, a
mov ebx, b

cmp eax, ebx
jge .L1

mov ecx, y
mov x, ecx
jmp .L0

.L1: