# Assembler

![Language: C](https://img.shields.io/badge/language-C-5C6B73)
![Standard: C89](https://img.shields.io/badge/standard-C89-5C6B73)
![Build: gcc and make](https://img.shields.io/badge/build-gcc%20%2B%20make-5C6B73)

A two-pass assembler for a 32-bit assembly language with R, I, and J instruction formats. It reads one or more `.as` source files, expands parameterless macros, builds a symbol table, and encodes every instruction as a 32-bit word. Code is laid out from address 100, four bytes per instruction, and data is placed immediately after the code. A successful run writes an object file and, when the program needs them, entry and external files.

The language below is the one implemented in this repository: 27 instructions, 32 registers, and the directives `.db`, `.dh`, `.dw`, `.asciz`, `.entry`, and `.extern`.

## Features

- Macro expansion with `mcro` / `mcroend` before either assembly pass
- A symbol table with code, data, entry, and external attributes
- Separate instruction and data images, with data symbols relocated after the code
- Encoding of R, I, and J instructions, including branch distances and external references
- Object (`.ob`), entry (`.ent`), and external (`.ext`) output
- Several source files in one invocation, each assembled independently
- Diagnostics that name the source line, and removal of partial output after a failed file

## How it works

`main` handles each filename in turn. A file has to end in `.as`. For `prog.as` the stages are:

1. **Macro expansion.** `macro_expand` reads `prog.as` and writes `prog.am`. Macro definitions are stored in a linked list. A line whose only token is the name of a macro defined earlier in the file is replaced by that macro's body. The `.am` file is deleted before the process exits, whether assembly succeeds or fails.

2. **First pass.** The expanded file is classified line by line into a blank line, a comment, an instruction, or a directive. Labels are inserted into the symbol table. Instructions whose fields are already known — R-type instructions, I-type arithmetic and memory instructions, `jmp` through a register, and `hlt` — are encoded into the instruction image. Branches and jumps to labels are parsed and stored as placeholders. Numeric data and `.asciz` strings are appended to the data image, and a data label is recorded as an offset from the start of that image. After the file has been read, every data symbol's value is increased by the final instruction counter, so data addresses begin where the code ends.

3. **Second pass.** The expanded file is read again with the symbol table in place. For `beq`, `bne`, `blt`, and `bgt`, the byte distance from the branch to its target is written into the immediate field. For `jmp`, `la`, and `call` with a label, the symbol address is written into the word; an external symbol is encoded as address 0 and the use is recorded. A `.entry` directive marks an existing symbol. `.extern`, data, and string lines were completed in the first pass.

4. **Output.** On success, `write_output` creates `prog.ob` and, only when there is something to list, `prog.ent` and `prog.ext`. On failure, any object, entry, and external files for that input are deleted.

The instruction counter starts at 100 and grows by 4 for every instruction. The data counter starts at 0 and counts bytes. The two images are separate: the instruction image holds up to 1000 words, and the data image holds up to 1000 bytes.

```
address 100
    instruction word
    instruction word
    ...
final instruction counter
    data bytes (.db, .dh, .dw, .asciz)
```

## Source language

### Lines, comments, and labels

A source line may be at most 80 characters. A blank line is ignored. A line whose first non-whitespace character is `;` is a comment, and the comment occupies the whole line: a semicolon after an instruction is extra text and is rejected.

A label is a name with a colon attached (`NAME:`). Names are case-sensitive, begin with a letter, and continue with letters or digits, up to 31 characters. A label cannot be an instruction name, a directive name with or without the leading dot, `mcro`, `mcroend`, or the name of a macro. The same symbol cannot be defined twice.

A label on `.entry` or `.extern` is accepted and ignored. A label with no instruction or data directive is an error.

### Registers, immediates, and commas

Registers are `$0` through `$31`. An immediate is a signed decimal in the range -32768 to 32767. Commas separate operands. Spaces and tabs around tokens and commas are allowed.

### Instructions

Each instruction is one 32-bit word. The encoders place the fields as follows, and bits that no field covers stay 0:

```
R   31      26 25  21 20  16 15  11 10   6 5    0
    opcode     rs     rt     rd     funct  unused

I   31      26 25  21 20  16 15                 0
    opcode     rs     rt     immediate

J   31      26 25 24                            0
    opcode     f  address
```

`f` is 1 when a `jmp` operand is a register and 0 when the operand is a label. The address field then holds the register number, the label's address, or 0 for an external label.

| Mnemonic | Opcode | Funct | Operands |
| --- | ---: | ---: | --- |
| `add` | 0 | 1 | `$rs, $rt, $rd` |
| `sub` | 0 | 2 | `$rs, $rt, $rd` |
| `and` | 0 | 3 | `$rs, $rt, $rd` |
| `or` | 0 | 4 | `$rs, $rt, $rd` |
| `nor` | 0 | 5 | `$rs, $rt, $rd` |
| `move` | 1 | 1 | `$rs, $rd` (`rt` is 0) |
| `mvhi` | 1 | 2 | `$rs, $rd` (`rt` is 0) |
| `mvlo` | 1 | 3 | `$rs, $rd` (`rt` is 0) |
| `addi` | 10 | | `$rs, immediate, $rt` |
| `subi` | 11 | | `$rs, immediate, $rt` |
| `andi` | 12 | | `$rs, immediate, $rt` |
| `ori` | 13 | | `$rs, immediate, $rt` |
| `nori` | 14 | | `$rs, immediate, $rt` |
| `bne` | 15 | | `$rs, $rt, label` |
| `beq` | 16 | | `$rs, $rt, label` |
| `blt` | 17 | | `$rs, $rt, label` |
| `bgt` | 18 | | `$rs, $rt, label` |
| `lb` | 19 | | `$rs, immediate, $rt` |
| `sb` | 20 | | `$rs, immediate, $rt` |
| `lw` | 21 | | `$rs, immediate, $rt` |
| `sw` | 22 | | `$rs, immediate, $rt` |
| `lh` | 23 | | `$rs, immediate, $rt` |
| `sh` | 24 | | `$rs, immediate, $rt` |
| `jmp` | 30 | | `label` or `$register` |
| `la` | 31 | | `label` |
| `call` | 32 | | `label` |
| `hlt` | 63 | | none |

Branch distance is `target address - address of the branch`, in bytes, and it has to fit in the signed 16-bit immediate. The target has to be defined in the same file. A branch to an external symbol is rejected. `jmp`, `la`, and `call` may name an external symbol; the use is then listed in the `.ext` file.

### Directives

| Directive | Meaning |
| --- | --- |
| `.db n, ...` | One byte per value, range -128 to 255 |
| `.dh n, ...` | Two bytes per value, little-endian, range -32768 to 65535 |
| `.dw n, ...` | Four bytes per value, little-endian, range -2147483648 to 4294967295 |
| `.asciz "text"` | Printable ASCII (codes 32–126) followed by a zero byte. An empty string is valid |
| `.entry name` | The symbol must be defined in this file. Its address is written to the `.ent` file |
| `.extern name` | The symbol is supplied by another module. Its value in this file is 0 |

A data directive needs at least one number. A trailing comma, a missing comma, and text after the last operand are errors. Declaring the same external name again is allowed. A symbol cannot be both an entry and an external.

### Macros

```
mcro NAME
    add $1, $2, $3
    hlt
mcroend
```

A macro has no parameters. The name is a single token of at most 31 characters, and it must not be a reserved word or a macro name already used in the file. The `mcro` line holds the keyword and the name. The `mcroend` line holds only the keyword. A call is a line that contains only the macro name.

Definitions are collected in one forward scan, so a macro has to appear before it is used. The body is copied into the expanded source as plain text, and a macro call inside that body stays as written. If `mcroend` is absent, the assembler keeps the rest of the file as that macro's body and leaves those lines out of the expanded source.

## Build

The makefile calls `gcc` with `-pedantic -ansi -Wall` and `-Iheader_files`, so the sources are compiled as C89. `gcc` and `make` need to be on the path.

```bash
make
```

That produces `./assembler` in the project root, along with one `.o` file per translation unit. The makefile has no clean target. Those build products are listed in `.gitignore`.

## Usage

```bash
./assembler <file.as> [more.as ...]
```

Each argument is one source file. The program prints a short progress trace to standard output and returns 0 when every file succeeds. It returns 1 when the command line is wrong or any file fails. Later files are still assembled after an earlier failure. Output files are written beside the source, using the same directory and base name: `dir/prog.as` produces `dir/prog.ob`.

With no arguments the program prints:

```
Proper command is this: assembler <filename>
```

### Example

`tests/example_external.as` is a complete program: an imported routine, an exported entry point, a forward branch, and all three data directives.

```
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
```

```bash
./assembler tests/example_external.as
```

```
========================================
Processing file: tests/example_external.as
========================================
Macro expension SUCCEEDED for tests/example_external.as
========================================
First pass SUCCEEDED for tests/example_external.as
========================================
Second pass SUCCEEDED for tests/example_external.as
========================================
Output processing COMPLETED for tests/example_external.as
========================================
Output writing SUCCEEDED for tests/example_external.as
========================================
```

The run writes the three files below. The temporary `tests/example_external.am` file is removed before the program exits.

`tests/example_external.ob`:

```
20 15
0100 40 30 A0 04
0104 FF 00 A7 30
0108 08 00 A7 40
0112 00 00 00 80
0116 00 00 00 FC
0120 80 00 7F 00
0124 80 FF 7F 00
0128 00 00 80 FF
0132 FF FF 7F
```

`tests/example_external.ent`:

```
MAIN 0100
```

`tests/example_external.ext`:

```
PRINT 0112
```

### Output files

Addresses in all three files are decimal, printed in four digits. `0100` is address 100, and `0112` is address 112. Instruction bytes are two uppercase hexadecimal digits, least significant byte first.

The first line of the `.ob` file is the code size in bytes, then the data size in bytes. In the example that is `20 15`: five instructions (20 bytes) and 15 data bytes. Each following code line is the instruction address and its four bytes. Data continues at the final instruction counter, up to four bytes per line, and the last line is shorter when the data size is not a multiple of four.

| Address | Source | Encoded bytes |
| ---: | --- | --- |
| 100 | `move $5, $6` | `40 30 A0 04` |
| 104 | `andi $5, 255, $7` | `FF 00 A7 30` |
| 108 | `beq $5, $7, END` | `08 00 A7 40` |
| 112 | `call PRINT` | `00 00 00 80` |
| 116 | `END: hlt` | `00 00 00 FC` |
| 120 | start of `NUMBERS` | data, four bytes per line |

`END` is at address 116 and the branch is at 108, so the immediate is 8. That is the leading `08 00` on the branch line. `PRINT` is external, so `call` is encoded with address 0 and the `.ext` file records the instruction that used it.

The 15 data bytes are the `.db`, `.dh`, and `.dw` operands in order, each value little-endian. They are then split into rows of four, so a row can cross a directive boundary:

```
80 00 7F          .db  -128, 0, 127
00 80             .dh  -32768
FF 7F             .dh   32767
00 00 00 80       .dw  -2147483648
FF FF FF 7F       .dw   2147483647
```

The `.ent` file has one line per entry symbol, `name` and its address, in the order the symbols were defined. It is created only when at least one entry symbol exists. The `.ext` file has one line per use of an external symbol, `name` and the address of the instruction that referenced it. It is created only when at least one such use was recorded. A later successful run that no longer needs one of those files deletes the stale file.

An empty file, or a file that contains only comments, still succeeds and writes an object file whose only line is `0 0`.

## Project structure

| Path | Role |
| --- | --- |
| `main.c` | Checks arguments and runs macro expansion, both passes, output, and cleanup for every file |
| `macro.c` | Expands macros from a `.as` file into the temporary `.am` file |
| `macro_helpers.c` | Macro list, reserved-word test, and the 80-character line check |
| `classifier.c` | Splits a line into a label, a comment, an instruction, or a directive |
| `first_pass.c` | Builds the symbol table and encodes fields that are known before label resolution |
| `first_pass_helper.c` | Operation table, operand parsers, instruction and data encoders, symbol table |
| `second_pass.c` | Resolves branches, label operands, `.entry`, and external uses |
| `second_pass_helper.c` | Linked list of external-symbol uses |
| `write_output_files.c` | Writes `.ob`, `.ent`, and `.ext`, and deletes them after a failure |
| `header_files/` | Limits, shared types, and function declarations |
| `makefile` | Builds `assembler` with `gcc -pedantic -ansi -Wall` |
| `tests/` | Sample programs and the edge cases listed below |

`constants.h` holds the line, name, and image limits. `datatypes.h` holds the symbol, macro, external-use, and operation types. The other headers declare the functions in the table above.

| Test | What it covers |
| --- | --- |
| `tests/example_valid.as` | A macro, `.entry`, forward and backward branches, a register `jmp`, `.asciz`, and a 31-character label |
| `tests/example_external.as` | The example in this README |
| `tests/example_errors.as` | A collection of syntax errors. Assembly fails and no output file is left behind |
| `tests/edge_empty.as` | An empty file. The object file is `0 0` |
| `tests/edge_comments.as` | Comments only. Same empty object file |
| `tests/edge_macro_termination.as` | Extra text after `mcroend` |
| `tests/edge_branch_distance.as` | A branch to a label at the end of a 1000-instruction program |
| `tests/capacity_1000.as` | 1000 instructions, which fills the instruction image and is accepted |
| `tests/capacity_1001.as` | 1001 instructions, which is rejected |

## Error handling

Progress messages and diagnostics are both written to standard output. A diagnostic that comes from a source line includes that line's number. A failed file leaves no `.ob`, `.ent`, `.ext`, or `.am` behind, and the process exit status is 1. Other filenames on the same command line are still processed.

The checks are:

- **Invocation.** At least one argument is required, the name has to end in `.as`, and it has to be at most 255 characters. A missing file is reported when it cannot be opened.
- **Macros.** A line over 80 characters, a missing or illegal macro name, a duplicate macro, extra text on `mcro`, `mcroend`, or a call, and an allocation failure all fail expansion.
- **First pass.** Invalid or duplicate labels, a label with no statement, an unknown instruction or directive, a bad register, a missing comma, an immediate or data value out of range, a malformed `.asciz` string, and a full instruction or data image. The pass reports every such fault it finds, then fails the file.
- **Second pass.** An undefined branch or jump target, a branch to an external symbol, a branch distance outside -32768 to 32767, an `.entry` of an undefined symbol, and a symbol that is both entry and external. Each of these is reported, and then the file fails.

A repeated `.extern` for a name that is already external prints a notice (`Extern <name> in line <n> is already defined (no error)`) and assembly continues.

## Limits

| Item | Limit |
| --- | --- |
| Source line | 80 characters |
| Label or macro name | 31 characters |
| Input filename | suffix `.as`, at most 255 characters |
| Registers | `$0` through `$31` |
| Immediate | -32768 to 32767 |
| Branch distance | -32768 to 32767 bytes, and the target must be local |
| Instructions | 1000 words (4000 bytes), from address 100 |
| Data | 1000 bytes, placed after the code |
| `.db` / `.dh` / `.dw` | -128 to 255 / -32768 to 65535 / -2147483648 to 4294967295 |
| `.asciz` | printable ASCII, codes 32 through 126, then a zero byte |

## Author

[Tatiyana G](https://github.com/Tatiyana-TFG)
