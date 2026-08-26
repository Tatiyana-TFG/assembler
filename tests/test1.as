  mcro INIT
  addi $0,1,$1
  jmp START
  mcroend

  START: add $1,$2,$3
  INIT
  sub $3,$2,$4
  and $1,$2,$5
  or $1,$2,$6
  nor $1,$2,$7

  move $7,$8
  mvhi $8,$9
  mvlo $9,$10

  addi $1,10,$11
  subi $11,-5,$12
  andi $12,255,$13
  ori $13,16,$14
  nori $14,0,$15

  beq $1,$2,BNETARGET
  bne $1,$2,BLTTARGET
  blt $1,$2,BGTARGET
  bgt $2,$1,MEMORY

  BNETARGET: lb $1,0,$2
  sb $2,1,$3
  lw $3,4,$4
  sw $4,8,$5
  lh $5,2,$6
  sh $6,6,$7

  jmp EXT
  jmp $7
  la DATA
  call EXT
  hlt

  MEMORY: hlt
  BLTTARGET: hlt
  BGTARGET: hlt

  DATA: .db -128,255,0
  HALF: .dh -32768,65535
  WORD: .dw -2147483648,4294967295
  TEXT: .asciz "Hi!"

  .entry START
  .entry DATA
  .extern EXT
