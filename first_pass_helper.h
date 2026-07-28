#ifndef CLASSIFIER_HELPER_H
#define CLASSIFIER_HELPER_H

int add_symbol (Symbol_table * symbols, char *name, int val, int code,int dat, int ent, int ext);
int valid_label(char *name, Macro *mcro);
void save_DC(void);
void save_IC(void);

#endif

