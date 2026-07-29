#include <stdlib.h> 
#include <string.h>
#include "datatypes.h"
#include "macro_helpers.h"





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



    


/*
add_extern(){
    add_symbol(extern with att 0);
};
add_entry(){
    find_symbol(entry);
    add_attribute( add the entry att to the found symbol);
};
free_symbol(){};
*/
