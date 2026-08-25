#ifndef SECOND_PASS_HELPER_H
#define SECOND_PASS_HELPER_H
#include "datatypes.h"

int add_ext_use(Extern_table *externs, char *name, int address);
void free_externs(Extern_table *externs);

#endif