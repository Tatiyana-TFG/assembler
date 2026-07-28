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



int add_symbol(char *name, Symbol *head) {
    /* 1 for success 0 for faliure adding */
    find_symbol();


}
add_extern(){
    add_symbol(extern with att 0);
};
add_entry(){
    find_symbol(entry);
    add_attribute(/*add the entry att to the found symbol*/);
};
free_symbol(){};