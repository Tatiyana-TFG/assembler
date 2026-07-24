## Hardware
## CPU 
- 32 Registers

## Registers
- 0 - 31 bits
- 0 LSB

## RAM
- each memory cell is 1 byte (8 bits)
- 0 - (2^25) - (1) bits of memory 
- each memory address is represnted by 25 bits

## Neg and Pos && ASCII
- 2's compliment
- ASCII suppoeted

## Commands
- each command is 32 bits
- each command is 4 consecutive bytes in little -endian
- each command has an opcode - some commands have funct -> i need to read for example ( "add" -> translates to opcode 0 and funct 1 -> implement that)
- Register layout "R" Immidiate layer "I" Jump layout "J" - define the split of the 32 bit command


  - R = opcode|rs|rt|rd|funct
  - I = opcode|rs|rt|immed
  - J = opcode|reg|address


  ┌──────┬────────┬───────────────────────────┐
  │  Op  │ Format │ Operand shape parser sees │
  ├──────┼────────┼───────────────────────────┤
  │ add  │ R      │ reg, reg, reg             │
  ├──────┼────────┼───────────────────────────┤
  │ move │ R      │ reg, reg ← 2 regs, not 3! │
  ├──────┼────────┼───────────────────────────┤
  │ addi │ I      │ reg, immed, reg           │
  ├──────┼────────┼───────────────────────────┤
  │ beq  │ I      │ reg, reg, label           │
  ├──────┼────────┼───────────────────────────┤
  │ lw   │ I      │ reg, immed, reg           │
  ├──────┼────────┼───────────────────────────┤
  │ jmp  │ J      │ label or reg              │
  ├──────┼────────┼───────────────────────────┤
  │ la   │ J      │ label only                │
  ├──────┼────────┼───────────────────────────┤
  │ hlt  │ J      │ nothing                   │
  └──────┴────────┴───────────────────────────┘

# Parser 
    - results in struct

# Encoder
    - writes into memory
    