  #ifndef MACRO_HELPERS_H
  #define MACRO_HELPERS_H

  #include <stdio.h>
  #include "datatypes.h"
  
  int line_too_long(const char *line, FILE *fp);
  int find_macro(Macro *head, char *name); 
  int is_reserved(char *name);
  Macro *add_macro(Macro *head, char *name, int *error);
  void add_line(Macro *mcro, char *text, int *error);
  void print_macro(Macro *mcro, FILE *out);
  void free_macros(Macro *mcro);


  #endif