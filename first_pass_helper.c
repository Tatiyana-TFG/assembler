#include <stdlib.h> 
#include <string.h>
#include "datatypes.h"
#include "macro_helpers.h"
#define NUMBER_OF_OP_ELEMENTS 27;


static const Operation operation_info[] = {
      /* R format */
      {"add",   0,  1, 'R'},
      {"sub",   0,  2, 'R'},
      {"and",   0,  3, 'R'},
      {"or",    0,  4, 'R'},
      {"nor",   0,  5, 'R'},
      {"move",  1,  1, 'R'},
      {"mvhi",  1,  2, 'R'},
      {"mvlo",  1,  3, 'R'},

      /* I format */
      {"addi", 10, -1, 'I'},
      {"subi", 11, -1, 'I'},
      {"andi", 12, -1, 'I'},
      {"ori",  13, -1, 'I'},
      {"nori", 14, -1, 'I'},
      {"bne",  15, -1, 'I'},
      {"beq",  16, -1, 'I'},
      {"blt",  17, -1, 'I'},
      {"bgt",  18, -1, 'I'},
      {"lb",   19, -1, 'I'},
      {"sb",   20, -1, 'I'},
      {"lw",   21, -1, 'I'},
      {"sw",   22, -1, 'I'},
      {"lh",   23, -1, 'I'},
      {"sh",   24, -1, 'I'},

      /* J format */
      {"jmp",  30, -1, 'J'},
      {"la",   31, -1, 'J'},
      {"call", 32, -1, 'J'},
      {"hlt",  63, -1, 'J'}
  };


const Operation *find_op(char *line){
    char *p = line;
    int i;
    char *tok;
    char line_copy[MAX_LINE];
    int const op_elements = NUMBER_OF_OP_ELEMENTS; 
    /* skip whitespaces*/
    while (*p== ' ' || *p== '\t') p++;
    strcpy (line_copy,p);
    tok = strtok(line_copy,"\t \n"); /* get the operatation name into token and compare in while loop*/
    if (tok == NULL)
        return NULL;
    for (i=0; i < op_elements; i++){
        if (strcmp(tok,operation_info[i].name)==0){ /* find the array element with the operation name*/
            return &operation_info[i]; 
        }
    }   return NULL; /* return null pointer if dosent exist*/

}

int valid_label(char *name, Macro *mcro){ /*1 valid -  0 invalid*/
char *p;
if (!((name[0] >= 'a' && name[0] <= 'z') || (name[0]>= 'A' && name [0]<='Z')))
    return 0;
for (p = name +1 ; *p != '\0'; p++){
    if (!((*p >= 'a' && *p  <= 'z') || (*p  >= 'A' && *p  <= 'Z') || (*p >= '0' && *p <= '9'))) /* check all is  or num letters*/
        return 0;
}
if (!is_reserved(name)){
    if(!find_macro(mcro,name)){
        return 1;
        } return 0;
    } return 0;
}

int add_symbol(Symbol_table *table,char *name, int value, int code, int dat, int ent, int ext) { /*success added on 1*/
    if (find_symbol(table,name) == 0){
        Symbol *curr = malloc(sizeof(Symbol));
        if (!curr){ 
            printf("Error: cannot allocate space for '%s',\n",name);
            return 0;
        } 
           
        strcpy(curr->name,name);
        curr->value=value;
        curr->is_code = code;
        curr->is_dat = dat;
        curr->is_ent = ent;
        curr->is_ext = ext;
        curr->next = NULL;
        if (table->head == NULL)
            table->head = table->tail = curr; /* only node in the list both head and tail*/
        else {
            table ->tail ->next = curr; /* more nodes exist go to the tail and set its next to be curr*/
            table->tail = curr;/* then set the tail to point at the last added node*/
        }
        return 1;
    } printf("Error: symbol '%s' already defined\n", name);
    return 0; /* 0 for not added */
}

int find_symbol(Symbol_table *symbols, char *name) {/* foud on 1*/
        Symbol *p = symbols ->head;
        while (p != NULL){
            if (strcmp(p-> name,name)== 0)
                return 1;
            p = p->next;
        }
        return 0;
    } 

int get_operand(char *p, char *op) {
char *tok;
strtok(p, " \t \n");
tok = strtok(NULL," \t \n");
if (!tok) return 0;
strcpy(op,tok);
return 1;
}

int parse_three_regs();
int parse_two_regs();
int parse_reg_number_reg();
int parse_two_regs_label();
int parse_label_or_reg();
int parse_label();
int validate_hlt();
/*
free_symbol(){};
*/
