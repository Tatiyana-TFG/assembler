
#ifndef WRITE_OUTPUT_FILES_H
#define WRITE_OUTPUT_FILES_H
#include "datatypes.h"
void remove_output_files(const char *filename);
int write_output (const char *filename, int IC,int DC, const unsigned int *inst_image, const unsigned char *data_image, const Symbol_table *symbols, const Extern_table *externs);
int write_entry(const char *filename, const Symbol_table *symbols);
int write_extern(const char *filename , const Extern_table *externs);
int write_ob(const char *filename, int IC , int DC, const unsigned int inst_image[], const unsigned char data_image[]);
#endif
