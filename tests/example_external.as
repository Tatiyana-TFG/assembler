.extern PRINT
.entry MAIN
MAIN: move $5,$6
andi $5,255,$7
beq $5,$7,END
call PRINT
END: hlt
NUMBERS: .db -128,0,127
.dh -32768,32767
.dw -2147483648,2147483647
