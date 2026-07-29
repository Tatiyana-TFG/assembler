#ifndef CLASSIFIER_HELPER_H
#define CLASSIFIER_HELPER_H

int add_symbol(Symbol_table *table,char *name, int *value, int code, int dat, int ent, int ext);
int valid_label(char *name, Macro *mcro);
int find_symbol(Symbol_table *symbols, char *name);
int get_operand(char *p, char *op);


#endif

