#include <string.h>
#include <stddef.h>
#include <stdlib.h> 
#include "macro_helpers.h"

int line_too_long(const char *line, FILE *fp) {
    int c;
    if (strchr(line, '\n') == NULL && !feof(fp)) 
    {
        while ((c = fgetc(fp)) != '\n' && c != EOF);
        return 1;
    }
    return 0;
}
   
int find_macro(Macro *head, char *name){                                                                                                                          
        Macro *p = head;                                                                                                       
        while (p != NULL) {                                                                                                    
            if (strcmp(p->name, name) == 0)                                                                                    
                return 1;                                                                                                      
            p = p->next;                                                                                                       
        }                                                                                                                      
        return 0;                  
    }

Macro *add_macro (Macro *head, char *name, int *error){
    Macro *new_mcro = malloc(sizeof(Macro));
    if (new_mcro== NULL){
        *error =1;
        return head;
    }
    strcpy(new_mcro->name, name);
    new_mcro->head = NULL;
    new_mcro->tail = NULL; 
    new_mcro ->next = head;
    head = new_mcro;
    *error =0;
    return head;
}

void add_line (Macro *mcro, char *token, int *error){
    Line *new_line = malloc(sizeof (Line));
    if (new_line== NULL) {
        *error =1;
        return;
    }
    strcpy (new_line->text,token);
    new_line->prev = NULL;              /*new nodes prev pointing to null*/

      if (mcro->head != NULL) {/* if head is point to something we have at least 1 line*/
          mcro->head->prev = new_line;   /*go to head and to its prev and set it to be the new line*/
      } else {
          mcro->tail = new_line;           /* first line ever point tail to new line*/
      }
      mcro->head = new_line;                /* if we inserted a new line point head to the new lien*/
      *error = 0;
  }
void print_macro(Macro *mcro, FILE *out){
    Line *p = mcro->tail;
    while (p!= NULL){
        fputs(p->text,out);
        p= p ->prev;
    }

}
 void free_macros(Macro *mcro) {
      while (mcro != NULL) {
          Macro *next_mcro = mcro->next;
          Line *p = mcro->tail;
          while (p != NULL) {
              Line *next_line = p->prev;
              free(p);
              p = next_line;
          }
          free(mcro);
          mcro = next_mcro;
      }
  }
  
int is_reserved(char *name) 
{
    static const char * forbiden_names[] = {
        "add", "sub", "and", "or", "nor",
        "move", "mvhi", "mvlo",
        "addi", "subi", "andi", "ori", "nori",
        "bne", "beq", "blt", "bgt",
        "lb", "sb", "lw", "sw", "lh", "sh",
        "jmp", "la", "call", "hlt",
        ".db", ".dw", ".dh", ".asciz", ".entry", ".extern",
        "mcro", "mcroend",
        NULL};
      
const char **p;

    for (p = forbiden_names ; *p != NULL; p++) { /*check if macro name is in the forbidden name array*/ 
                if (strcmp(*p, name) ==0){ /*if is then error and stop */
                return 1;
                }
            }
            return 0;
}

