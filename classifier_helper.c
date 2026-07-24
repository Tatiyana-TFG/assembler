#include "macro.h"
#include <stdlib.h> 
#include <string.h>

void save_DC(void) {

};

void save_IC(void){

};

void is_extent(){

};

add_symbol();
find_symbol();
add_extern(){
    add_symbol(extern with att 0);
};
add_entry(){
    find_symbol(entry);
    add_attribute(/*add the entry att to the found symbol*/);
};
free_symbol(){};