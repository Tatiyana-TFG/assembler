#include "constants.h"

#ifndef DATATYPES_H
#define DATATYPES_H

typedef struct Symbol {
    char name[MAX_LINE];
    int value;
    int is_code;
    int is_dat;
    int is_ent;
    int is_ext;
    struct Symbol *next;
} Symbol;


typedef struct Symbol_table {
    Symbol *head;
    Symbol *tail;
} Symbol_table;

typedef struct Operation {
    char name[MAX_LINE];
    int opcode;
    int funct;
    char foramt;
    Operands operands_type;
} Operation;

typedef enum Operands{
    THREE_REG,
    TWO_REG,
    REG_NUMBER_REG,
    TWO_REGS_LABEL,
    LABEL_OR_REG,
    LABEL_ONLY,
    NO_OPERANDS
} Operands;

typedef struct Line {
      char text[MAX_LINE];
      struct Line *prev;
  } Line;

typedef struct Macro {
      char name[MAX_NAME];
      Line *head;
      Line *tail;
      struct Macro *next;
  } Macro;


typedef enum 
{ 
    LINE_EMPTY, LINE_COMMENT, LINE_INSTRUCTION, LINE_DIRECTIVE
}
    line_type; 


typedef enum 
{ 
    EXTERN, ENTRY, DATA, STRING, BAD_DIR
}
    directive_type; 

#endif
