#include "first_pass_helper.h"
#include "datatypes.h"
#include "macro.h"
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>

int is_label(char **line, char *name, Macro *mcro){ /*check if it is a label definition with :*/
  int len;
  char line_copy[MAX_LINE];
  char *token;

  char *start =  *line; /*point to the same place line points to*/
  char *p= *line; /*point to the same place line points to*/

  /* 1 = valid lable 
  0 = : but no lable 
  -1 = bad lable */
  skip_whitespaces(&p);
  start = p;
  
  while (*p != '\0' && *p != '\t' && *p != '\n' && *p != ' ' && *p != ':') p++; /* find first token*/
  if (*p == ':'){ /*check if : exists*/
    len = p - start; /* counter of how many chars the token is */
    if (len == 0 || len >= MAX_NAME) { /* check length*/
      return -1; /* invalid length*/
    }
    memcpy(name,start,len); /*copy to name from start at length of len*/
    name [len] = '\0'; /*add terminator \0 at the end*/

    *line = p+1;
    skip_whitespaces(line);
  
    strcpy (line_copy,*line); /* copy content to line copy*/
    token = (strtok (line_copy, " \t \n")); /* get the  name after label*/
    if (token != NULL && (strcmp(token, ".entry") == 0 || strcmp(token, ".extern") == 0)){ /* if entry ext*/
      return 1; /* label validation is ignored if matched */
  }

  if (valid_label(name,mcro)){ /*check if label is valid by other definitions*/
    return 1; /* label is good */
    } else 
    return -1; /*failed at the validity check*/
  
  } else 
  return 0; /* : exists but no label */
}

line_type classify_line(char *line){
  while (*line == ' ' || *line == '\t') line++; /* skip whitespaces*/
  if (*line == '\0' || *line == '\n') return LINE_EMPTY;
  if (*line == ';') return LINE_COMMENT;
  if (*line == '.')  return LINE_DIRECTIVE;
  return LINE_INSTRUCTION;
      
}

directive_type classify_dir (char *line, int line_num){
  char line_copy[MAX_LINE];
  char *token, *line_p;
  strcpy(line_copy, line); /* copy original line to line copy*/
  line_p = line_copy; /* point at line copy */

  while (*line_p == ' ' || *line_p == '\t') line_p++; /* skip whitespaces*/
  token = strtok(line_p, "\t \n"); /*point to first token*/

  if (token == NULL){ /* check if token exists*/
    printf("Error: missing information in line %d\n", line_num);
    return BAD_DIR;
  }

  if (strcmp(token,".extern") == 0) return EXTERN;
  if (strcmp(token,".entry") == 0) return ENTRY;
  if (strcmp(token,".dh") == 0 || strcmp(token,".db") == 0 ||  strcmp(token,".dw") == 0)return DATA;
  if (strcmp(token,".asciz") == 0) return STRING;

return BAD_DIR;
}
