#include <stdio.h>
#include <string.h>
#include "datatypes.h"
#include "macro.h"
#include "macro_helpers.h"
#include "second_pass.h"
#include "first_pass_helper.h"
#include "write_output_files.h"
#include "second_pass_helper.h"

int first_pass(char *am, Macro *mcro_head, int *IC,int *DC, unsigned int *inst_image, unsigned char *data_image, Symbol_table *symbols);

int main(int argc, char *argv[]) {
    int had_error = 0;
    int i;
    if (argc < 2) {
        printf("Proper command is this: assembler <filename>\n"); /* */
        return 1;
    } 
    
    for (i = 1; i < argc; i++) { /* add checks for .am in the file recived  */
        char src[256], am[256]; /* maybe should also be dynamic */
        int name_length;
        unsigned char data_image[IMAGE_CAPACITY] = {0};
        unsigned int inst_image[IMAGE_CAPACITY] = {0};
        Extern_table externs = {NULL, NULL};
        Symbol_table symbols = { NULL, NULL };   /* head, tail */
        int IC = 100; /* instruction counter starts at 100 per spec */
        int DC = 0;   /* data counter starts at 0 */
        Macro *mcro_head = NULL;

        name_length = (int)strlen(argv[i]);
        if ((name_length < 3) || strcmp(argv[i] + name_length -3, ".as") !=0) { /* check suffix .as */
            printf("Error: Filename must end with  '.as' \n");
            had_error = 1;
            goto cleanup;
        }

        if (name_length >= (int)sizeof(src)) {
            printf("Error: filename must not exceed 255 chars\n");
            had_error = 1;
            goto cleanup;
        }

        sprintf(src, "%s", argv[i]);
        sprintf(am,  "%s", argv[i]);  /* am  = "prog.am" */
        am[name_length -1] = 'm'; /* change to .as -> .am*/
        if (macro_expand(src, am, &mcro_head) == 0){ /* send src and pointer to dst */
            had_error = 1;
            goto cleanup;
        }

        if (first_pass(am,mcro_head,&IC,&DC, inst_image, data_image, &symbols) == 0){
            had_error = 1;
            goto cleanup;
        }
        
        if (second_pass(am,mcro_head,inst_image,&symbols,&externs) ==0) {
            had_error = 1;
            goto cleanup;
        }
        
        if (!write_output_files(argv[1], IC,inst_image, data_image, &symbols, &externs)) {
            had_error = 1;
            goto cleanup;
        }

    /*reate .ob .ent .ext */

    cleanup:
        free_macros(mcro_head);
        free_externs(&externs);
        free_symbols(&symbols);
} 
return had_error;
/* for loop */


}
