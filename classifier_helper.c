#include <stdlib.h> 
#include <string.h>
#include "datatypes.h"
#include "macro_helpers.h"

void save_DC(void) {

};

void save_IC(void){

};

void is_extent(){

};

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



int add_symbol(Symbol_table *table,char * name, int value, int code, int dat, int ent, int ext) {

    Symbol *curr = malloc(sizeof(Symbol));
    if (!curr) return 0;
    strcpy(curr->name,name);
    curr->value=value;
    curr->is_code = code;
    curr->is_dat = dat;
    curr->is_ent = ent;
    curr->is_ext = ext;
    *head = curr;
    return 1;
}
int find_symbol(char *name, Symbol **head);


}
add_extern(){
    add_symbol(extern with att 0);
};
add_entry(){
    find_symbol(entry);
    add_attribute(/*add the entry att to the found symbol*/);
};
free_symbol(){};