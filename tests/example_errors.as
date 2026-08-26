DUP: hlt
DUP: hlt
add $32,$1,$2
add $1,$2
hlt $1
add $1 $2,$3
.unknown 4
.asciz "missing closing quote
.db 256

; Additional malformed input cases.
.db 1,
.dh
.dw 12abc
.asciz
.asciz "ok" extra
