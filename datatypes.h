#include "constants.h"

#ifndef DATATYPES_H
#define DATATYPES_H

typedef struct Symbol {
    char name[MAX_LINE];
    int value;
    int attribute;
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
    IS_DATA, IS_CODE, IS_ENTRY, IS_EXTERNAL, IS_DATA_ENTRY, IS_DATA_EXTERNAL
} 
    attribue; 

    typedef enum 
{ 
    LINE_EMPTY, LINE_COMMENT, LINE_INSTRUCTION, LINE_DIRECTIVE
}
    line_type; 

#endif
