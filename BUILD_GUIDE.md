# Building Your Assembler — Explained From Zero

> A beginner-friendly guide to the 20465 final project (two-pass assembler in C).
> Your own scratch notes live in `Instructions.md`; this file is the walkthrough.

---

## 1. What is this project, really?

A CPU (the chip in a computer) can't read words like `add` or `jump`. It only
understands **numbers** — long strings of 0s and 1s. But humans hate writing 0s
and 1s. So people invented **assembly language**: short text codes like
`add $3,$5,$9` that a human can read.

An **assembler** is a **translator**. It reads the human text and turns it into
the numbers the CPU wants.

> Think of a chef who writes recipes in shorthand ("2c flr, 1tsp slt"). A machine
> in the kitchen only accepts exact robot commands. The assembler is the helper
> who reads the shorthand and writes out the precise robot commands.

**Your job:** write that translator, in C. Input = a text file of assembly.
Output = files full of the translated numbers.

You run it like this:

```
assembler program.as
```

and it produces output files (we'll get to which ones).

---

## 2. The pretend computer you're translating *for*

This project invents an imaginary computer. You need to know its rules, because
that's what you translate *into*.

**Registers** — the CPU has **32 little boxes** to hold numbers while it works,
named `$0`, `$1`, … `$31`. Like 32 labeled cups on a desk. Math happens in these
cups.

**Memory (RAM)** — a very long street of **mailboxes**, each numbered (an
"address"). Each mailbox holds **1 byte** (8 bits = 8 zeros/ones). Your program
gets loaded into memory **starting at mailbox number 100**.

**One instruction = 32 bits = 4 bytes.** So each instruction fills **4 mailboxes
in a row**.

**Little-endian** — when a 4-byte number is stored across 4 mailboxes, the *last*
(least important) byte goes in the *first* (lowest) mailbox. It feels backwards.
Just a rule to remember. Example: the number whose bytes are `00 65 48 40` gets
stored in memory as `40 48 65 00`. (You'll see this exact case below.)

---

## 3. How one line of text becomes numbers (full worked example)

Let's translate this tiny program by hand. Once you can do it on paper, you know
what your C code must do.

```
MAIN:  add $3,$5,$9
       hlt
```

### Step A — figure out where each instruction lives in memory

Memory starts at 100. Each instruction is 4 bytes.

| address | line |
|---------|------|
| 100 | `add $3,$5,$9` |
| 104 | `hlt` |

So `MAIN` is a **label** — a nickname for the address 100.

### Step B — translate `add $3,$5,$9` into 32 bits

Every instruction has an **opcode**: a number that says *which* operation it is.
`add` is opcode **0**. Because lots of operations share opcode 0, `add` also has
a **funct** number to tell them apart: funct **1**. (You get these from a table —
included below.)

`add` is an **"R-type"** instruction. R-type splits its 32 bits into these slots:

```
[ opcode ][ rs ][ rt ][ rd ][ funct ][ unused ]
  6 bits   5     5     5      5         6 bits
```

For `add $3,$5,$9`, the rule is `rd = rs + rt`, and the operands are read as
**rs, rt, rd in that order**. So:

- rs = 3
- rt = 5
- rd = 9
- opcode = 0, funct = 1

Now write each number in binary, in its slot:

```
opcode  rs     rt     rd     funct  unused
000000  00011  00101  01001  00001  000000
```

Glue them together (32 bits), then cut into 4 bytes of 8 bits:

```
00000000  01100101  01001000  01000000
   0x00      0x65       0x48      0x40
```

Store little-endian (last byte first): **`40 48 65 00`**. Those 4 numbers go in
mailboxes 100, 101, 102, 103. **You just translated one instruction.**

### Step C — translate `hlt`

`hlt` means "stop." Opcode **63**, no operands, everything else 0. In binary
that's `111111` then 26 zeros. As bytes, little-endian: **`00 00 00 FC`** in
mailboxes 104–107.

That's the entire idea of the project. Everything else is handling more
instruction shapes, labels that point forward, and bookkeeping.

---

## 4. Why "two passes"? (the heart of the project)

Here's the catch. What if you write:

```
       jmp  END      ← "jump to END"
       ...
END:   hlt           ← END is defined down here
```

When the assembler reaches `jmp END`, it doesn't *yet* know what address `END`
is — `END` is defined later in the file. This is a **forward reference**.

Solution: read the file **twice**.

- **Pass 1:** go through once and just *record where every label lives* (build a
  phone book of label → address). Don't worry about filling in jump targets yet.
- **Pass 2:** go through again. Now the phone book is complete, so you can fill in
  every `jmp END` with END's real address.

That phone book is called the **symbol table**. It's the most important data
structure you'll build.

> Like reading a textbook twice: first pass you note "Chapter 5 starts on page
> 80"; second pass you can resolve every "see Chapter 5" into "see page 80."

---

## 5. The full pipeline (what your program does, start to finish)

```
   program.as
       │
   STAGE 0: expand macros   ─────►  program.am
       │
   STAGE 1: first pass  (build the symbol table / phone book)
       │
   STAGE 2: second pass (fill in label addresses, finish the numbers)
       │
       ▼
   program.ob   program.ent   program.ext
```

### Stage 0 — macros (a copy-paste shortcut)

A **macro** is a named block of lines. Define it once, use the name many times,
and the assembler pastes the block in. Example:

```
mcro GEN
   add  $1,$2,$3
   hlt
mcroend
```

Now anywhere the line `GEN` appears, replace it with those two lines. Read
`program.as`, write the expanded result to **`program.am`**. Stages 1 and 2 work
on the `.am` file.

### Stage 1 — first pass

Keep two counters:

- **IC** = instruction counter, starts at **100** (where instructions go).
- **DC** = data counter, starts at **0** (counts data bytes).

Go line by line:

- Empty line or comment (starts with `;`) → ignore.
- A **label** (`MAIN:` …) → write it in the symbol table with the current address.
- An **instruction** → record the parts you already know, then `IC += 4`.
- A **data directive** (`.db`, `.dh`, `.dw`, `.asciz` — see §6) → store the bytes,
  advance DC.
- `.extern NAME` → note that NAME lives in another file.

When pass 1 ends, instructions occupy 100 up to some final IC. Data goes *after*
the instructions, so you shift every data label's address forward by that amount.
(The spec calls the final value **ICF**; you add it to each data symbol.)

### Stage 2 — second pass

Now the symbol table is complete. Go through the instructions again and:

- Fill in jump/branch targets using the symbol table.
- For a **branch** (`beq`, `bne`, `blt`, `bgt`), the number stored is
  **relative**: `target_address − this_instruction's_address`.
- For `jmp`/`la`/`call`, the number is the label's **actual address**.
- If a target is an **external** label (in another file), store 0 and write a note
  to the `.ext` file.
- If a label is used but never defined → **error**.

If there were no errors at all, write the output files.

---

## 6. The building blocks you'll meet in the source

### Directives (lines starting with `.`) — these place *data*, not instructions

| you write | meaning | bytes used |
|-----------|---------|-----------|
| `.db 7,-5` | store these as single **b**ytes | 1 byte each |
| `.dh 300,-1` | store as **h**alf-words | 2 bytes each |
| `.dw 120056` | store as full **w**ords | 4 bytes each |
| `.asciz "abc"` | store the text as ASCII codes | 1 byte per char **+ one 0 byte at the end** |
| `.entry K` | "let other files use my label K" | — |
| `.extern val1` | "val1 is defined in some other file" | — |

### Labels (the nicknames)

- Start with a letter, then letters/digits, **max 31 characters**.
- **Case-sensitive** (`Hello` ≠ `hello`).
- Where defined, ends with `:` → `LOOP:`.
- Can't reuse an instruction or directive name.

### Line rules

- A source line is at most **80 characters** (not counting the newline).
- Four kinds of line: **empty**, **comment** (`;`), **directive** (`.`),
  **instruction**.

---

## 7. The instruction table (your reference)

Three layouts. **R** and **I** and **J** just slice the 32 bits differently:

```
R:  [opcode 6][rs 5][rt 5][rd 5][funct 5][unused 6]
I:  [opcode 6][rs 5][rt 5][immed 16]
J:  [opcode 6][reg 1][address 25]
```

| name | opcode | funct | type | what it does |
|------|:------:|:-----:|:----:|--------------|
| add  | 0 | 1 | R | `rd = rs + rt` |
| sub  | 0 | 2 | R | `rd = rs − rt` |
| and  | 0 | 3 | R | bitwise and |
| or   | 0 | 4 | R | bitwise or |
| nor  | 0 | 5 | R | bitwise nor |
| move | 1 | 1 | R | copy register → register |
| mvhi | 1 | 2 | R | copy high half |
| mvlo | 1 | 3 | R | copy low half |
| addi | 10 | – | I | `rt = rs + number` |
| subi | 11 | – | I | `rt = rs − number` |
| andi | 12 | – | I | and with a number |
| ori  | 13 | – | I | or with a number |
| nori | 14 | – | I | nor with a number |
| bne  | 15 | – | I | branch if `rs ≠ rt` |
| beq  | 16 | – | I | branch if `rs = rt` |
| blt  | 17 | – | I | branch if `rs < rt` |
| bgt  | 18 | – | I | branch if `rs > rt` |
| lb / sb | 19 / 20 | – | I | load / store a byte |
| lw / sw | 21 / 22 | – | I | load / store a word |
| lh / sh | 23 / 24 | – | I | load / store a half-word |
| jmp  | 30 | – | J | jump to a label or register |
| la   | 31 | – | J | load an address into `$0` |
| call | 32 | – | J | jump and remember where to return |
| hlt  | 63 | – | J | stop |

**⚠ The trap that costs students points — operand order in the text is NOT always
the field order in binary:**

| you write | who's who |
|-----------|-----------|
| `add $3,$5,$9` | rs=3, rt=5, **rd=9** (destination is *last*) |
| `move $23,$2` | **rd=23 (destination is *first*!)**, rs=2 |
| `addi $9,-45,$8` | rs=9, **the number sits in the middle**, rt=8 |
| `blt $5,$24,loop` | rs=5, rt=24, target=loop |
| `lh $9,34,$2` | rs=9 (base), 34 (offset), rt=2 |

Always encode by **slot name** (rs, rt, rd), never by "first/second/third operand."

---

## 8. The output files (for input `program.as`)

| file | when created | what's inside |
|------|-------------|---------------|
| `program.am` | always | the source after macro expansion |
| `program.ob` | if no errors | the machine code, as hex bytes |
| `program.ent` | only if `.entry` was used | `label  address`, one per entry |
| `program.ext` | only if an external label was used | `label  address`, one per use |

**`.ob` format:**

- First line: two decimal numbers — total bytes of instructions, then total bytes
  of data.
- Then rows of: 4-digit address, then up to 4 hex bytes (little-endian):

```
52 17
0100 40 48 65 00
0104 FB FF 22 35
...
```

---

## 9. Where to start: datatypes + parser

Everything above is the *what*. Here's the *how to begin*. Start with these two
because everything else uses them.

Two jobs, two files:

- **`datatypes.h`** defines your **nouns** — the *things* your program talks
  about. "What is an instruction? What is a symbol? What kinds of lines exist?"
  These are `struct`s and `enum`s. No logic, just shapes.
- **`parser`** is the **verb** — it takes one ugly line of text and *fills in* one
  of those structs.

```
"MAIN: add $3,$5,$9\n"   ──parser──►   struct { label="MAIN", op=add, rs=3, rt=5, rd=9 }
   (messy text)                          (clean data your encoder can use)
```

Everything downstream (passes, encoder) reads the **struct**, never the raw text
again. That's why you start here.

### Step 1 — `datatypes.h`: define the nouns

**(a) What kind of line is it?**

```c
typedef enum { LINE_EMPTY, LINE_COMMENT, LINE_INSTRUCTION, LINE_DIRECTIVE } line_type;
```

**(b) Which binary layout?** (used by the encoder — see §10 for why)

```c
typedef enum { FORMAT_R, FORMAT_I, FORMAT_J } inst_format;
```

**(c) The operation lookup table.** Every instruction name maps to an opcode,
maybe a funct, and a format:

```c
typedef struct {
    char *name;        /* "add"      */
    int   opcode;      /* 0          */
    int   funct;       /* 1, or -1 if the instruction has no funct */
    inst_format format;/* FORMAT_R   */
} operation;
```

You'll fill a real array of these — `{"add",0,1,FORMAT_R}, {"sub",0,2,FORMAT_R},
...` — straight from the table in §7.

**(d) One parsed instruction.** Every field any format might use:

```c
typedef struct {
    int  opcode, funct;
    inst_format format;
    int  rs, rt, rd;        /* register numbers; use -1 for "unused" */
    long immed;             /* I-type number */
    long address;           /* J-type address */
    char operand_label[32]; /* if a label is the target, store its NAME here;
                               you can't know its address until pass 2 */
} instruction;
```

> Why `operand_label` matters: when the parser sees `jmp END`, it can't fill in
> `address` — `END` isn't defined yet. So it stashes the *name* `"END"` and pass 2
> looks it up. This one field is the whole reason two passes exist.

**(e) A symbol** (for the symbol table):

```c
typedef struct {
    char name[32];
    int  value;        /* its address */
    int  is_code, is_data, is_entry, is_external; /* attributes, 0 or 1 */
} symbol;
```

Start with `line_type`, `inst_format`, and `operation`. Add the rest when the
parser needs them.

### Step 2 — the parser: two layers

Your **old `parser.h` had the right instinct** — names like `parse_3reg`,
`parse_2reg`, `parse_reg_imm_reg` each read a different operand shape. Keep that
idea. But you need a layer *above* them.

**Layer 1 — the "front desk"** (one function for the whole line). Handles what
*every* line has in common, in order:

1. **Is there a label?** First word ends in `:`? Save it, move past it.
2. **Grab the operation word** (`add`, `jmp`, `.db`).
3. **Directive or instruction?** Starts with `.` → directive path.
4. **Look up the operation** in your `operation` table → opcode, funct, format.
5. **Hand the rest of the line** (`$3,$5,$9`) to the right Layer-2 reader.

**Layer 2 — the operand readers** (your old `parse_3reg` etc.). These read *just
the operands* and fill the struct. The tiny helpers you already wrote in
`praser.c` are the reusable pieces — you just swap what they read:

| your old helper | keep it, but change it to… |
|-----------------|----------------------------|
| `skip_ws` | keep as-is |
| `read_comma` | keep as-is (commas work the same) |
| `check_end` | keep as-is |
| `read_var` (read `A`–`F`) | rewrite → `read_register` (read `$0`–`$31`) |
| `read_val` (read a `double`) | rewrite → `read_number` (read a whole `int`) |

So `parse_3reg` becomes: `read_register, read_comma, read_register, read_comma,
read_register, check_end`. You're reassembling your old building blocks in a new
order. Reuse the plumbing, rewrite the two leaves.

### A concrete order to build & test (don't write it all at once)

1. **Fix `datatypes.h`** — get `line_type`, `inst_format`, `operation`,
   `instruction` compiling. Nothing else yet.
2. **Write the `operation` table** — the array of every instruction from §7.
3. **Write `read_register`** — give it `"$5, $9"`, get back `5`, pointer advanced.
   Test it alone in a `main`.
4. **Write `read_number`** — `"-45, $8"` → `-45`. Test alone.
5. **Rebuild one Layer-2 reader**, e.g. `parse_3reg`. Test with `add $3,$5,$9`.
6. **Write Layer 1** (label? + operation lookup + dispatch). Test with a full line.
7. Add the other Layer-2 shapes one by one.

Test each step with a 5-line `main` before moving on.

---

## 10. Why do I need `inst_format` (R/I/J)?

The parser doesn't need it. The **encoder** does. Two different questions:

- **Parsing** asks: *"what operand shape do I read?"* — 3 registers? reg + number
  + reg? two regs + a label?
- **Encoding** asks: *"how do I pack the bits into 32?"* — R / I / J layout.

These are **not the same thing**:

```
addi $9,-45,$8   →  I-format  →  reads: reg, number, reg
beq  $5,$24,loop →  I-format  →  reads: reg, reg, label
```

Both are I-format (same bit layout) but **different operand shapes** (different
parser). So:

- You **cannot** dispatch the parser on R/I/J — `beq` and `addi` are both I but
  parse differently. Dispatch on the *specific instruction* instead.
- But when you pack the bits, `beq` and `addi` go into the **same slots**
  `[opcode][rs][rt][immed]`. *That's* what `inst_format` tells the encoder:

```c
switch (inst.format) {
    case FORMAT_R: /* pack rs,rt,rd,funct */ break;
    case FORMAT_I: /* pack rs,rt,immed     */ break;
    case FORMAT_J: /* pack reg,address     */ break;
}
```

Without the enum, the encoder would re-guess the layout from the opcode every time
("is opcode 0 or 1? then R…") — the same knowledge in two places. Put `format` in
your `operation` table once; parser and encoder both read from one source of truth.

| field in your `operation` table | used by | answers |
|--------------------------------|---------|---------|
| operand shape (how you dispatch the parser) | parser | "read 3 regs vs reg,imm,reg…" |
| `inst_format` (R/I/J) | encoder | "pack bits into which slots" |

Rule of thumb: a field earns its place if it stops you from re-deriving the same
fact later. `inst_format` does.

---

## 11. Grader requirements (don't lose easy points)

- Compile clean with `gcc -ansi -pedantic -Wall`.
- Ship a `makefile`.
- **No external libraries.**
- Split into several `.c` / `.h` files, each with a clear job.
- Collect every error with its line number, print them all, and only write output
  files if there were **no** errors.
