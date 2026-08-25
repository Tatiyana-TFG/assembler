# Assembler Codebase Review and Fix Guide

This document compares the current assembler implementation with the requirements in
`etc/מטלת 14.pdf`.

It does **not** modify the implementation. Every code block below is a proposed fix or
implementation sketch. Apply and test the fixes one group at a time.

## Working-tree note

While this review file was being written, `main.c` and
`header_files/first_pass_helper.h` acquired uncommitted changes outside this document's
patch. Those implementation changes were left untouched. This review describes the
current versions, including the incomplete `sprintf` edit currently in `main.c`.

## Current status

The project contains useful foundations:

- The complete operation table for the 27 instructions.
- R-, I-, and J-instruction encoders.
- Operand parsers for registers, immediate values, and labels.
- Numeric data encoding for `.db`, `.dh`, and `.dw`.
- Linked-list structures for macros and symbols.
- A partial macro expander and partial first pass.

It is not yet an end-to-end assembler because the build currently fails, the first pass
is incomplete, and the second pass and output writers do not exist.

---

# 1. Clarification: overlong lines are skipped, but the file still incorrectly continues

## Where

`macro.c:40-43` and `macro.c:105-108`

## Current code

```c
if (line_too_long(line,in)){
    printf("Error: Line %d too long\n" , line_num);
    continue;
}
```

## What the current code does correctly

`line_too_long` drains the unread remainder of the physical source line. The `continue`
then moves to the next source line. That part is correct: it prevents the remainder of a
long line from being mistaken for a new assembly statement.

## What is still wrong

The error is forgotten after `continue`. At the end of `macro_expand`, the function
always does this:

```c
*mcro_head=mcro;
return 0;
```

Therefore an overlong line is removed from the `.am` file, but macro expansion reports
success. `main` then runs the first pass on an incomplete `.am` file. The PDF requires a
macro-stage error to stop processing that input file and move to the next input file.

There are two separate operations:

1. Drain/skip the bad physical line and continue scanning so it is read safely.
2. Remember that the file contains an error so the first pass is not started.

## Proposed fix

Use a file-level error flag while still continuing to the next line:

```c
int had_error = 0;

/* ... inside either reading loop ... */
if (line_too_long(line, in)) {
    printf("Error: line %d is longer than 80 characters\n", line_num);
    had_error = 1;
    continue;
}

/* ... after the entire file has been scanned ... */
fclose(in);
fclose(out);

if (had_error) {
    free_macros(mcro);
    *mcro_head = NULL;
    return -1;
}

*mcro_head = mcro;
return 0;
```

This preserves your desired behavior—continue to the next line—while stopping the first
pass for the bad input file.

---

# 2. Clarification: the current line-length test can accept 81 characters

## Where

`header_files/constants.h:4` and `macro_helpers.c:6-13`

## Current code

```c
#define MAX_LINE 83
```

```c
int line_too_long(const char *line, FILE *fp) {
    int c;
    if (strchr(line, '\n') == NULL && !feof(fp)) {
        while ((c = fgetc(fp)) != '\n' && c != EOF);
        return 1;
    }
    return 0;
}
```

## Why this differs from an exact 80-character check

`fgets(line, 83, fp)` can store 82 input bytes plus the terminating `\0`.

For a line containing 81 source characters followed by `\n`, `fgets` stores all 81
characters and the newline. Because `strchr(line, '\n')` succeeds, the function returns
`0`, even though the PDF limit is 80 source characters.

There is another edge case: if the final line has no newline, `fgets` may set EOF after
reading it. The `!feof(fp)` condition can then prevent the function from reporting that
an overlong final line is too long.

The current function detects “the buffer did not contain a newline before it filled.” It
does not directly test “the source statement contains more than 80 characters.” Those
conditions are close, but with an 83-byte buffer they are not identical.

## Proposed fix

Keep a buffer large enough to detect the 81st source character and explicitly count the
characters:

```c
#define MAX_SOURCE_LINE 80
#define LINE_BUFFER_SIZE 82  /* 81 characters + terminating null */
```

```c
int line_too_long(const char *line, FILE *fp)
{
    size_t length = strlen(line);
    int has_newline = length > 0 && line[length - 1] == '\n';
    int c;

    if (has_newline) {
        return (length - 1) > MAX_SOURCE_LINE;
    }

    /* No newline: either this is the final line, or more characters remain. */
    if (length > MAX_SOURCE_LINE) {
        while ((c = fgetc(fp)) != '\n' && c != EOF) {
            /* Drain the remainder of the physical line. */
        }
        return 1;
    }

    c = fgetc(fp);
    if (c == EOF) {
        return 0; /* A legal final line without a newline. */
    }

    /* Another character exists, so the line is longer than the buffer showed. */
    while (c != '\n' && c != EOF) {
        c = fgetc(fp);
    }
    return 1;
}
```

An alternative is to use a larger temporary input buffer and simply compare the length
before the newline with 80. The important point is to test the character count, not just
the presence of a newline.

---

# 3. Clarification: `LABEL:` by itself is currently accepted

## Where

`first_pass.c:36-47`

## Current control flow

```c
has_label = is_label(&p, label_name, mcro_head);
/* ... */
type = classify_line(p);

switch (type) {
    case LINE_EMPTY:
    case LINE_COMMENT:
        continue;
```

## What happens with `LABEL:`

1. `is_label` returns `1` and advances `p` past the colon.
2. The remaining text is empty or only whitespace/newline.
3. `classify_line(p)` returns `LINE_EMPTY`.
4. The switch immediately continues without reporting an error.
5. The label is not added to the symbol table, but the source line is still silently
   accepted.

A label definition is an optional prefix of an instruction or directive statement. A
label alone does not identify an instruction address or a data address, so it should not
be accepted silently.

The same issue occurs with `LABEL: ; comment`: after the label is removed, the remainder
is classified as a comment and ignored.

## Proposed fix

Check this immediately after classification:

```c
type = classify_line(p);

if (has_label && (type == LINE_EMPTY || type == LINE_COMMENT)) {
    printf("Error: label '%s' has no instruction or data directive in line %d\n",
           label_name, line_num);
    error_flag++;
    continue;
}
```

This is different from labels placed before `.entry` or `.extern`: the PDF says such a
leading label has no meaning and may be ignored with an optional warning. Handle that
case inside the directive logic.

---

# 4. Clarification: extra text after a macro invocation is currently discarded

## Where

`macro.c:133-139`

## Current code

```c
} else {
    if (token && (find_macro(mcro,token))){
        print_macro(mcro,out);
    } else {
        fputs(line, out);
    }
}
```

## Example of the problem

Given:

```text
mcro GEN
add $1,$2,$3
mcroend

GEN unexpected_text
```

`token` is `GEN`, so the macro is expanded. The `unexpected_text` token is never checked
and disappears from the `.am` file.

The checks using `strtok(NULL, ...)` at `macro.c:81` and `macro.c:111` only validate the
`mcro NAME` and `mcroend` definition lines. They do not validate a normal macro-use line.

## Proposed fix

First fix macro lookup so it returns the matching macro. Then check that there is no
second token:

```c
Macro *found;
char *extra;

/* tmp has already been tokenized and token is the first field. */
found = token != NULL ? find_macro(mcro, token) : NULL;

if (found != NULL) {
    extra = strtok(NULL, " \t\n");
    if (extra != NULL) {
        printf("Error: extra text after macro '%s' in line %d\n",
               token, line_num);
        had_error = 1;
        continue;
    }
    print_macro(found, out);
} else {
    fputs(line, out);
}
```

---

# 5. Build failure in `classifier.c`

## Where

`classifier.c:47-49`

## Current code

```c
char *token, *line_p;
strcpy(line_copy, line);
char *line_p = line_copy;
```

## Problem

`line_p` is declared twice. The second declaration is also placed after an executable
statement, which violates strict ANSI C90 declaration rules. `printf` is used without
including `<stdio.h>`.

## Proposed fix

```c
#include <stdio.h>
```

```c
directive_type classify_dir(char *line, int line_num)
{
    char line_copy[MAX_LINE];
    char *token;
    char *line_p;

    strcpy(line_copy, line);
    line_p = line_copy;

    /* existing classification logic */
}
```

---

# 6. Function-declaration cleanup is still incomplete

## Where

`header_files/first_pass_helper.h`, `first_pass_helper.c:377-619`, and
`first_pass.c:123-154`

## Problems

- Prototypes for `parse_encode_data`, `parse_string`, `is_valid_data_value`,
  `is_valid_op_loop`, and `encode_data` have now been added to the header, but they need
  to remain synchronized with the definitions.
- `store_ic_value` is still declared as returning `unsigned int`, implemented as `void`,
  and used as if it returns success/failure.
- `int add_extern();` at `first_pass_helper.c:123` is an unfinished declaration.
- `#include <datatypes.h>` was added before the include guard. Project headers should be
  included inside the guard, normally with quotes.

## Proposed header structure and declarations

```c
#ifndef FIRST_PASS_HELPER_H
#define FIRST_PASS_HELPER_H

#include "datatypes.h"

int parse_encode_data(char *p, int line_num,
                      unsigned char *data_image, int *DC);
int parse_string(char *p, int line_num,
                 unsigned char *data_image, int *DC);
int is_valid_data_value(long value, int num_of_bytes);
long *is_valid_op_loop(char *p, int line_num,
                       int num_of_bytes, int *data_array_count);
void encode_data(const long *data_op_array, int data_array_count,
                 int num_of_bytes, unsigned char *data_image, int *DC);
int store_ic_value(unsigned int encoded_val, int *IC,
                   unsigned int inst_image[]);

#endif
```

## Proposed `store_ic_value`

```c
int store_ic_value(unsigned int encoded_val, int *IC,
                   unsigned int inst_image[])
{
    int index = (*IC - 100) / 4;

    if (index < 0 || index >= IMAGE_CAPACITY) {
        return 0;
    }

    inst_image[index] = encoded_val;
    *IC += 4;
    return 1;
}
```

Remove the unfinished `int add_extern();` declaration after implementing external-symbol
handling in the symbol-table module.

---

# 7. `first_pass` needs its own header

## Where

`main.c:7`

## Current code

`main.c` manually declares the entire `first_pass` signature.

## Problem

Manual declarations easily become inconsistent with the definition. Each public module
should expose its interface through a header.

## Proposed file: `header_files/first_pass.h`

```c
#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "datatypes.h"

int first_pass(char *am, Macro *mcro_head, int *IC, int *DC,
               unsigned int *inst_image, unsigned char *data_image,
               Symbol_table *symbols);

#endif
```

Then include this header in both `main.c` and `first_pass.c`.

---

# 8. `main` does not explicitly return success

## Where

`main.c:62`

## Proposed fix

```c
return 0;
```

Add it immediately before the closing brace of `main`.

---

# 9. Input filenames are handled incorrectly

## Where

`main.c:26-27`

## Current code

```c
sprintf(src, "%s", argv[i]);
sprintf(am,  "%s, argv[i]);
```

## Problem

The first line now correctly preserves the command-line source filename, but the second
line has an unterminated string literal and will not compile. It also does not construct
the required `.am` filename. Both `sprintf` calls can overflow the fixed 256-byte
buffers.

The PDF shows source filenames, including `.as`, as command-line arguments. The program
should preserve `test.as` as its input and derive `test.am` from it.

## Proposed approach

Validate the `.as` suffix, copy the source name as-is, and replace the suffix when
constructing output names:

```c
size_t length = strlen(argv[i]);

if (length < 4 || strcmp(argv[i] + length - 3, ".as") != 0) {
    printf("Error: input file '%s' must end with .as\n", argv[i]);
    continue;
}

if (length >= sizeof(src)) {
    printf("Error: input filename is too long: %s\n", argv[i]);
    continue;
}

strcpy(src, argv[i]);

if (length + 1 > sizeof(am)) {
    printf("Error: output filename is too long: %s\n", argv[i]);
    continue;
}

strcpy(am, argv[i]);
strcpy(am + length - 3, ".am");
```

Use the same suffix-replacement helper for `.ob`, `.ent`, and `.ext`.

---

# 10. State leaks from one input file into the next

## Where

`main.c:10-20`; the file loop starts at `main.c:24`

## Problem

`IC`, `DC`, instruction/data images, and all symbol tables are initialized only once.
When several files are provided, later files inherit addresses and symbols from earlier
files. The PDF requires each input file to be assembled independently.

## Proposed structure

```c
for (i = 1; i < argc; i++) {
    int IC = 100;
    int DC = 0;
    unsigned char data_image[IMAGE_CAPACITY];
    unsigned int inst_image[IMAGE_CAPACITY];
    Symbol_table symbols;
    Macro *mcro_head = NULL;

    symbols.head = NULL;
    symbols.tail = NULL;
    memset(data_image, 0, sizeof(data_image));
    memset(inst_image, 0, sizeof(inst_image));

    /* Expand, run both passes, and write this file's outputs. */

    free_macros(mcro_head);
    free_symbols(&symbols);
}
```

In ANSI C90, declarations must appear at the beginning of their block, before executable
statements.

---

# 11. Macro lookup expands the wrong macro

## Where

`macro_helpers.c:16-24` and `macro.c:135-136`

## Current behavior

`find_macro` returns only `1` or `0`. If it returns `1`, `macro.c` passes the head of the
macro list to `print_macro`. Because new macros are inserted at the head, invoking an
older macro prints the newest macro instead.

## Proposed replacement

```c
Macro *find_macro(Macro *head, const char *name)
{
    Macro *p = head;

    while (p != NULL) {
        if (strcmp(p->name, name) == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}
```

All callers should test the returned pointer and pass that exact pointer to
`print_macro`.

---

# 12. Leading whitespace before a label is rejected

## Where

`first_pass.c:34-36` and `classifier.c:6-35`

## Problem

`is_label` is called before whitespace is skipped. For:

```text
    LOOP: add $1,$2,$3
```

the first character seen by `is_label` is a space, so no label is recognized. Later the
instruction parser sees `LOOP:` as the operation name.

## Proposed fix

```c
p = line;
skip_whitespaces(&p);
has_label = is_label(&p, label_name, mcro_head);
```

Alternatively, put the whitespace skip at the beginning of `is_label`, but be consistent
about whether that function is allowed to move the caller's pointer when no label exists.

---

# 13. Numeric data falls through into the string case

## Where

`first_pass.c:148-165`

## Current code shape

The `case DATA:` branch has no `break` or `continue` after successful numeric-data
encoding, so it falls directly into `case STRING:`.

For labeled numeric data, it then tries to add the same label a second time. For unlabeled
numeric data, it reaches the string case but does not actually parse a string.

## Proposed fix

```c
case DATA:
    if (has_label) {
        if (!add_symbol(symbols, label_name, *DC, 0, 1, 0, 0)) {
            error_flag++;
            continue;
        }
    }

    if (!parse_encode_data(p, line_num, data_image, DC)) {
        error_flag++;
    }
    continue;
```

---

# 14. `.asciz` is never encoded

## Where

`first_pass.c:159-165` and `first_pass_helper.c:512-576`

## Problem

`parse_string` exists, but the `STRING` case never calls it.

## Proposed fix

```c
case STRING:
    if (has_label) {
        if (!add_symbol(symbols, label_name, *DC, 0, 1, 0, 0)) {
            error_flag++;
            continue;
        }
    }

    if (!parse_string(p, line_num, data_image, DC)) {
        error_flag++;
    }
    continue;
```

## Additional string validation

The PDF requires visible/printable ASCII. Check each character before storing it:

```c
if ((unsigned char)*string_start < 32 ||
    (unsigned char)*string_start > 126) {
    printf("Error: non-printable character in string in line %d\n", line_num);
    return 0;
}
```

---

# 15. `.extern` and `.entry` are modeled as separate symbol tables

## Where

`main.c:18-20` and `first_pass.c:131-146`

## Problem

The PDF describes one symbol table with attributes such as code, data, external, and
entry. Separate tables prevent important conflict checks. For example, the same name can
be added once as a local symbol and once as external without either table noticing.

The current `add_symbol` also rejects repeated `.extern NAME` and `.entry NAME`, but the
PDF explicitly says repeated declarations of the same kind are allowed.

## Proposed symbol behavior

```c
int declare_external(Symbol_table *table, const char *name)
{
    Symbol *symbol = get_symbol(table, name);

    if (symbol != NULL) {
        if (symbol->is_ext) {
            return 1; /* Repeated .extern is legal. */
        }
        printf("Error: symbol '%s' is already defined locally\n", name);
        return 0;
    }

    return add_symbol(table, name, 0, 0, 0, 0, 1);
}
```

`.entry` should be handled in the second pass:

```c
int mark_entry(Symbol_table *table, const char *name)
{
    Symbol *symbol = get_symbol(table, name);

    if (symbol == NULL) {
        printf("Error: entry symbol '%s' is not defined\n", name);
        return 0;
    }
    if (symbol->is_ext) {
        printf("Error: external symbol '%s' cannot also be entry\n", name);
        return 0;
    }

    symbol->is_ent = 1; /* Repeating this assignment is harmless and legal. */
    return 1;
}
```

---

# 16. `.extern` and `.entry` operands are not fully validated

## Where

`first_pass_helper.c:125-132`

## Current code

```c
int get_operand(char *p, char *op) {
    char *tok;
    strtok(p, " \t \n");
    tok = strtok(NULL," \t \n");
    if (!tok) return 0;
    strcpy(op,tok);
    return 1;
}
```

## Problems

- It does not validate that the operand is a legal label.
- It does not reject a second operand or other trailing text.
- It modifies the source line with `strtok`.

## Proposed approach

After advancing past the directive name, use the same label reader used for instruction
operands, and require the end of the line:

```c
skip_whitespaces(&p);
if (!read_label(&p, operand, line_num, mcro_head)) {
    return 0;
}
skip_whitespaces(&p);
return check_end(&p, line_num);
```

It may be cleaner to write a dedicated `parse_single_label_directive` that does not rely
on `strtok`.

---

# 17. Data-symbol addresses are not relocated after pass 1

## Where

The required step is missing after the reading loop at `first_pass.c:173-176`.

## Problem

During pass 1, a data label correctly receives its current `DC`. However, the final
machine image puts all data after all instructions. The PDF therefore requires adding
the final instruction counter, `ICF`, to every data symbol.

## Proposed helper

```c
void relocate_data_symbols(Symbol_table *table, int ICF)
{
    Symbol *symbol = table->head;

    while (symbol != NULL) {
        if (symbol->is_dat) {
            symbol->value += ICF;
        }
        symbol = symbol->next;
    }
}
```

Call it only after a successful first pass:

```c
if (error_flag) {
    return -1;
}

*ICF = *IC;
*DCF = *DC;
relocate_data_symbols(symbols, *ICF);
return 0;
```

This likely requires changing the `first_pass` interface so it clearly returns or fills
`ICF` and `DCF`.

---

# 18. Image writes need capacity checks

## Where

`first_pass_helper.c:606-623`; string data has a check, but numeric data and instruction
storage do not.

## Problem

An input containing too many instructions or numeric data values can write beyond the
fixed arrays.

## Numeric-data check

Check before encoding:

```c
long required = (long)data_array_count * num_of_bytes;

if (required > IMAGE_CAPACITY - *DC) {
    printf("Error: data image capacity exceeded in line %d\n", line_num);
    free(data_op_array);
    return 0;
}
```

## Instruction check

Perform the index check in `store_ic_value` before writing, as shown in section 6.

Decide whether `IMAGE_CAPACITY` means bytes or instructions and name the constants
accordingly, for example:

```c
#define DATA_IMAGE_CAPACITY 1000
#define INSTRUCTION_CAPACITY 250
```

---

# 19. The second pass is completely missing

## Where

There is no `second_pass.c` or `second_pass.h`.

## Required behavior

The second pass must rescan the `.am` file and:

1. Process `.entry` declarations.
2. Resolve conditional-branch target labels.
3. Resolve `jmp`, `la`, and `call` target labels.
4. Reject undefined symbols.
5. Reject an external symbol used by a conditional branch.
6. Encode zero for an external symbol used by a J instruction.
7. Record every J-instruction address that uses an external symbol.
8. Continue scanning after errors and suppress outputs if any error occurs.

## Branch calculation

```c
offset = target_symbol->value - current_instruction_address;

if (offset < -32768 || offset > 32767) {
    printf("Error: branch offset is out of range in line %d\n", line_num);
    error_flag++;
} else {
    inst_image[instruction_index] &= 0xFFFF0000U;
    inst_image[instruction_index] |= (unsigned int)offset & 0xFFFFU;
}
```

## J-address calculation

```c
if (target_symbol->is_ext) {
    address = 0;
    add_external_use(external_uses,
                     target_symbol->name,
                     current_instruction_address);
} else {
    address = target_symbol->value;
}

inst_image[instruction_index] &= 0xFE000000U;
inst_image[instruction_index] |= (unsigned int)address & 0x01FFFFFFU;
```

When `jmp` uses a register, its address field was already completed in pass 1 and should
not be treated as a symbol reference.

---

# 20. Output-file generation is completely missing

## Where

There is no output module and `main.c` stops after the first pass.

## Required files

- `.ob` after every successful assembly.
- `.ent` only if at least one entry exists.
- `.ext` only if at least one external symbol is actually used by a J instruction.

## Object header

```c
fprintf(out, "%d %d\n", ICF - 100, DCF);
```

## Instruction bytes

```c
byte0 = instruction & 0xFFU;
byte1 = (instruction >> 8) & 0xFFU;
byte2 = (instruction >> 16) & 0xFFU;
byte3 = (instruction >> 24) & 0xFFU;

fprintf(out, "%04d %02X %02X %02X %02X\n",
        address, byte0, byte1, byte2, byte3);
```

## Entry and external lines

```c
fprintf(ent, "%s %04d\n", symbol->name, symbol->value);
fprintf(ext, "%s %04d\n", use->name, use->address);
```

The data bytes must follow the instruction bytes without gaps, four bytes per output row
where possible. The final row may contain fewer than four bytes.

Create these files only after both passes report zero errors. Consider deleting stale
`.ob`, `.ent`, and `.ext` files from an earlier run before processing a source file, so a
failed new run cannot leave misleading old outputs.

---

# 21. Symbol memory is never freed

## Where

`main.c` calls `free_macros`, but no symbol-table cleanup function exists. A commented
`free_symbol` placeholder appears near the end of `first_pass_helper.c`.

## Proposed fix

```c
void free_symbols(Symbol_table *table)
{
    Symbol *symbol = table->head;

    while (symbol != NULL) {
        Symbol *next = symbol->next;
        free(symbol);
        symbol = next;
    }

    table->head = NULL;
    table->tail = NULL;
}
```

Call it after every source file, including error paths. Add equivalent cleanup for the
external-use list needed by pass 2.

---

# 22. The current module organization is too broad

## Where

`first_pass_helper.c` contains the operation table, symbol-table functions, lexical
parsing, operand parsing, data parsing, and binary encoding.

## Problem

The PDF asks for modules separated by responsibility, and code quality/documentation is
a significant part of the grade. A single large helper module makes testing and
maintenance difficult.

## Suggested organization

```text
main.c
macro.c / macro.h
classifier.c / classifier.h
operations.c / operations.h
parser.c / parser.h
symbol_table.c / symbol_table.h
encoder.c / encoder.h
first_pass.c / first_pass.h
second_pass.c / second_pass.h
output.c / output.h
```

This reorganization should happen after the build and core behavior are stable, to avoid
mixing a large file move with functional debugging.

---

# 23. Makefile and required submission examples are missing

## Where

No Makefile, `.as` examples, expected outputs, or error-output demonstrations exist in
the repository.

## Proposed Makefile shape

```make
CC = gcc
CFLAGS = -Wall -ansi -pedantic -Iheader_files
OBJ = main.o macro.o macro_helpers.o classifier.o first_pass.o \
      first_pass_helper.o second_pass.o output.o

assembler: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o assembler

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) assembler
```

Adjust the object list to match the final modules. The submitted build must compile on
Ubuntu with no warnings.

## Minimum testing checklist

- The complete PDF example, compared byte-for-byte with its `.ob`, `.ent`, and `.ext`.
- All 27 operations.
- `.db`, `.dh`, `.dw`, and `.asciz`.
- Forward and backward branches.
- Internal and external J targets.
- Repeated `.entry` and `.extern`.
- Several input files in a single command.
- Leading/trailing whitespace and comma variations.
- Lines of exactly 80 and 81 characters.
- Invalid labels, duplicate labels, undefined symbols, and reserved names.
- Invalid register numbers, immediate values, operand counts, and commas.
- Missing/extra string quotes and non-printable characters.
- Instruction/data image capacity boundaries.

The PDF also requires at least three representative input/output/error demonstrations
for submission.

---

# Recommended implementation order

1. Fix all compilation errors and produce a warning-free executable.
2. Fix macro lookup, macro-invocation validation, and line-length error propagation.
3. Move all per-file state inside the command-line loop.
4. Fix leading-whitespace labels, numeric-data fall-through, and `.asciz` encoding.
5. Replace the separate external/entry tables with one coherent symbol table.
6. Relocate data symbols at the end of pass 1 and add capacity checks.
7. Test pass 1 against the symbol addresses and partial encodings in the PDF.
8. Implement and test the second pass.
9. Implement `.ob`, `.ent`, and `.ext` output.
10. Add cleanup, Makefile, test cases, and final documentation.
