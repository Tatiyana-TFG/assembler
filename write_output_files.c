#include <stdio.h>
#include <string.h>
#include "datatypes.h"
#include "first_pass_helper.h"
#include "second_pass_helper.h"

int write_output (const char *filename, int IC,int DC, const unsigned int *inst_image, const unsigned char *data_image, const Symbol_table *symbols, const Extern_table *externs){
    char ob_filename [MAX_NAME];
    char ent_filename [MAX_NAME];
    char ext_filename [MAX_NAME];
    int length;


    sprintf(ob_filename, "%.*s.ob", length - 3, filename);
    sprintf(ent_filename, "%.*s.ent", length - 3, filename);
    sprintf(ext_filename, "%.*s.ext", length - 3, filename);
    
    if (!write_object_file(ob_filename, IC, DC, inst_image, data_image))
        return 0;
    
    if (!write_entry_file(ent_filename, symbols))
        return 0;

    if (!write_extern_file(ext_filename, externs))
        return 0;

    return 1;
}

int write_entry(const char *filename, const Symbol_table *symbols);
int write_extern(const char *filename , const Extern_table *externs);
int write_ob(const char *filename, int IC , int DC, const unsigned int inst_image[], const unsigned char data_image[]);

}