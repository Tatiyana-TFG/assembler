Assembler test inputs

Build from the project root:
gcc -Wall -ansi -pedantic -Iheader_files *.c -o assembler

Run:
./assembler examples/example_valid.as
./assembler examples/example_external.as
./assembler examples/example_errors.as

The first two files should produce .am and .ob files, plus .ent/.ext when applicable.
The error file should print line-numbered errors and must not produce .ob/.ent/.ext files.

Additional edge cases are merged into example_valid.as and example_errors.as.
The valid file also tests whitespace around commas, case-sensitive labels, a
31-character label, empty strings, jmp through a register, and forward/backward
branches. The error file also tests trailing/missing data values, malformed numbers,
missing string operands, and extra string text.
edge_empty.as and edge_comments.as test files with no assembler statements.
edge_macro_termination.as tests extra text after mcroend.
edge_branch_distance.as tests the largest feasible branch span under IMAGE_CAPACITY.

Run multiple source files together:
./assembler examples/example_valid.as examples/example_external.as examples/example_errors.as

A branch offset of +/-32767 cannot be reached with the current 1000-instruction image
capacity; the largest feasible span is tested instead.

Capacity tests use real three-register add instructions:
./assembler examples/capacity_1000.as   # should succeed
./assembler examples/capacity_1001.as   # should report image capacity full
