#include "constants.h"

#ifndef DATATYPES_H
#define DATATYPES_H

typedef struct Symbol {
    char name[MAX_LINE];
    char value[MAX_LINE];
    char attribute [MAX_LINE];
    struct Symbol *next;
} Symbol;


typedef struct Symbol_table {
    Symbol *head;
    Symbol *tail;
} Symbol_table;


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
    line_type; /* how does the parser check this? */

#endif
