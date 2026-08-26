#include <stdio.h>
#include <string.h>
#include "datatypes.h"
#include "constants.h"
#include "first_pass_helper.h"
#include "second_pass_helper.h"
#include "write_output_files.h"

int write_output (const char *filename, int IC,int DC, const unsigned int *inst_image, const unsigned char *data_image, const Symbol_table *symbols, const Extern_table *externs){
    char ob_filename [MAX_FILENAME];
    char ent_filename [MAX_FILENAME];
    char ext_filename [MAX_FILENAME];
    int length;

    length = (int) strlen(filename);

    sprintf(ob_filename, "%.*s.ob", length - 3, filename);
    sprintf(ent_filename, "%.*s.ent", length - 3, filename);
    sprintf(ext_filename, "%.*s.ext", length - 3, filename);
    
    if (write_ob(ob_filename, IC, DC, inst_image, data_image) == 0) {
        printf ("Error: could not create %s\n", ob_filename);
        printf("========================================\n");
        return 0;
    }
    if (write_entry(ent_filename, symbols) == 0) {
        printf ("Error: could not create %s\n", ent_filename);
        printf("========================================\n");

        return 0;

    }
    if (write_extern(ext_filename, externs) == 0) {
        printf ("Error: could not create %s\n", ext_filename);
        printf("========================================\n");

        return 0;
    }
    /*else*/
    
    printf("Output processing COMPLETED for %s\n", filename);
    printf("========================================\n");

    return 1;

}

int write_entry(const char *filename, const Symbol_table *symbols) {
const Symbol *curr;
int has_entry;
FILE *out;

has_entry = 0;
for (curr = symbols -> head; curr != NULL; curr = curr -> next){ /* check if entry exists*/
    if (curr -> is_ent) {
        has_entry = 1;
        break;
    }
}

if (has_entry == 0) { /*if no entry exist dont create the file but return success*/
    return 1;
}
/* open file and write entries*/
out = fopen(filename,"w");
if (out == NULL) {
    return 0; 
}

for (curr = symbols -> head; curr != NULL; curr = curr -> next) {
    if (curr -> is_ent) {
        fprintf(out,"%s %04d\n", curr -> name, curr -> value); /* print ents*/
    }
}
if (fclose(out) == EOF) /* error in closing the file*/
    return 0;

return 1; /*printed ents*/

}

int write_extern(const char *filename , const Extern_table *externs) {
Extern_symbol *curr;
FILE *out;

if (externs -> head == NULL) {
    return 1; /* no externs we return success*/
}
/*open file and write externs */
out = fopen(filename,"w");

if (out == NULL) {
    return 0;
}

for (curr = externs -> head; curr != NULL; curr = curr -> next){ /*print when curr != null bc the list is only externs*/
    fprintf(out, "%s %04d\n", curr -> name, curr -> address);
}
if (fclose(out) == EOF)
    return 0;

return 1;


}

int write_ob(const char *filename, int IC , int DC, const unsigned int inst_image[], const unsigned char data_image[]){
int instruction_count;
int i, j;
unsigned int word;
unsigned int byte;
FILE *out;

    out = fopen(filename, "w"); /*open filename.ob*/
    if (out == NULL) {
        return 0;
    }

    instruction_count = (IC - 100)/4 ; /*how many 4 bytes of instructions in the image*/
    fprintf(out, "%d %d\n", IC - 100, DC);

    /* loop through instructions and print IC value -> inside loop throguh each line*/
    for (i=0; i< instruction_count; i++){
        fprintf(out,"%04d", 100 + (i * 4)); /* add number of bytes to 100 to get IC */
        
        word = inst_image[i]; /*hold 32 encoded bits*/

        for(j = 0; j < 4; j++) {/*print each byte in hex in little endian */
            byte = (word >> (j*8) & 0xFFU); /*shift selected byte and mask to get only the byte (mask is 1 in 8 bits) */
            fprintf(out, " %02X", byte);
        }
        fprintf(out,"\n");
    }
    /*print data image four bytes per line*/

    for(i = 0; i< DC; i+=4){
        fprintf(out,"%04d", IC +i); /* printinf from last IC address + size of first data */
            for(j = 0; j < 4 && i +j < DC; j++) { /* Print at most four bytes without passing the end of the final data row */
                fprintf(out," %02X", (unsigned int)data_image [i+j]);
            }
            fprintf(out,"\n");
        }
    if (fclose(out) == EOF) {
        return 0; /*error*/
    }
    return 1;
}
