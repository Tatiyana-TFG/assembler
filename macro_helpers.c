#include <string.h>
#include <stddef.h>
#include <stdlib.h> 
#include "macro_helpers.h"

/*
* Checks whether a source line exceeds the maximum allowed length
* If the line is too long, consumes the remaining characters from fp
* Returns 1 for an invalid line and 0 for a valid line
*/
int line_too_long(const char *line, FILE *fp) {
    int c;
    if (strchr(line, '\n') == NULL && (strlen(line) > 80)) 
    {
        while ((c = fgetc(fp)) != '\n' && c != EOF);
        /*consume extra chars*/
        return 1; /*line is too long*/
    }
    return 0; /*line valid*/
}

/*
* Searches the linked list of macros for name
* Returns the matching macro node or NULL when the macro is not defined
*/
Macro *find_macro(Macro *head, char *name){ /* if found return pointer*/                                                                                                                     
        Macro *p = head;                                                                                                       
        while (p != NULL) {                                                                                                    
            if (strcmp(p->name, name) == 0)                                                                                    
                return p;                                                                                                      
            p = p->next;                                                                                                       
        }                                                                                                                      
        return NULL;               
    }

/*
* Allocates and initializes a macro node, then adds it to the macro table
* Sets error to 1 if memory allocation fails
* Returns the updated table head
*/
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

/*
* Copies one source line into a macro's linked list
* Lines are linked in reverse insertion order so they can later be printed
* in their original source order
*/
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

/*
* Writes all source lines stored in mcro to the output stream in source order
*/
void print_macro(Macro *mcro, FILE *out){
    Line *p = mcro->tail;
    while (p!= NULL){
        fputs(p->text,out);
        p= p ->prev;
    }
}

/*Releases every macro node and every source-line node owned by the macro table*/
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
  
/*
* Checks whether name is reserved for an instruction, directive, or macro
* keyword. Returns 1 if reserved and 0 otherwise
*/
int is_reserved(char *name) /* 0 if not reserved 1 if reserved*/
{
    static const char * forbiden_names[] = {
        "add", "sub", "and", "or", "nor",
        "move", "mvhi", "mvlo",
        "addi", "subi", "andi", "ori", "nori",
        "bne", "beq", "blt", "bgt",
        "lb", "sb", "lw", "sw", "lh", "sh",
        "jmp", "la", "call", "hlt",
        ".db", ".dw", ".dh", ".asciz", ".entry", ".extern",
        "db", "dw", "dh", "asciz", "entry", "extern",
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
