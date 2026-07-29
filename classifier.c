#include "datatypes.h"
#include "macro.h"
#include "first_pass_helper.h"
#include <stdlib.h> 
#include <string.h>



int is_label(char **line, char *name, Macro *mcro){
  int len;
  char *start =  *line;
  char *p= *line;

  /* 1 = valid lable 
  0 = : but no lable 
  -1 = bad lable */

  while (*p != '\0' && *p != '\t' && *p != '\n' && *p != ' ' && *p != ':') p++; /* find first token*/
  if (*p == ':'){ /*check if : exists*/
    len = p - start; /* counter of how many chars the token is */
    if (len == 0 || len > MAX_NAME -1) { /* check length*/
      return -1; /* invalid label*/
    }
    memcpy(name,start,len); /*copy name from start length of len to name*/
    name [len] = '\0'; /*add terminator \0 at the end*/

    if (valid_label(name,mcro)){ /*check if label is valid by other definitions*/
      *line = p + 1; /*set line to point past the label for the rest of the check*/
      return 1; /* label is good */
    } else 
    return -1; /*failed at the validity check*/
  
  }
  else 
  return 0; /* : exists but no label */
}

line_type classify_line(char *line){
  while (*line == ' ' || *line == '\t') line++; /* skip whitespaces*/
  if (*line == '\0' || *line == '\n') return LINE_EMPTY;
  if (*line == ';') return LINE_COMMENT;
  if (*line == '.')  return LINE_DIRECTIVE;
  return LINE_INSTRUCTION;
      
    }

directive_type classify_dir (char *line){
  while (*line == ' ' || *line == '\t') line++; /* skip whitespaces*/
  if (strncmp(line, ".extern", 7) == 0) return EXTERN;
  if (strncmp(line, ".entry",  6) == 0) return ENTRY;
  if (strncmp(line, ".db", 3) == 0 || strncmp(line, ".dh", 3) == 0 || strncmp(line, ".dw", 3) == 0)     return DATA;
  if (strncmp(line, ".asciz", 6) == 0)  return STRING;
  return BAD_DIR;
}