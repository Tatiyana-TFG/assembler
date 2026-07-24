#include "datatypes.h"
#include "macro.h"
#include "classifier_helper.h"
#include <stdlib.h> 
#include <string.h>

line_type classify_line(char *line);
void save_DC(void);
void save_IC(void);

char *lable;
char tmp [MAX_LINE];
char *rest;
line_type type;



void is_lable(char *line){
  strcpy(tmp,line);
  if (strchr(line, ':')!= NULL){
    lable = strtok(tmp,":");
    char *rest = strtok(NULL,"\n");
    type= classify_line(rest);
    if( type == LINE_DIRECTIVE){
      save_DC();
      return;
      
    }else if (type == LINE_INSTRUCTION){
      save_IC();
      return;

    }else{ return; /* don't save*/  
    }
  }
  return;

}
  
  

line_type classify_line(char *line) 
{
/* skip leading whitespace */
      while (*line == ' ' || *line == '\t') line++;

      if (*line == '\0' || *line == '\n') return LINE_EMPTY;
      if (*line == ';')                   return LINE_COMMENT;
      if (*line == '.')  return LINE_DIRECTIVE;
      return LINE_INSTRUCTION;
}