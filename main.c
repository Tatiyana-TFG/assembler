#include <stdio.h>
#include <string.h>
#include "datatypes.h"
#include "macro.h"
#include "macro_helpers.h"

int first_pass(char *am, Macro *mcro_head, int *IC,int *DC, unsigned int *inst_image, unsigned char *data_image, Symbol_table *symbols, Symbol_table *externs, Symbol_table *entry);

int main(int argc, char *argv[]) {
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
        Symbol_table externs = { NULL, NULL };
        Symbol_table entry = { NULL, NULL };
        Symbol_table symbols = { NULL, NULL };   /* head, tail */
        int IC = 100; /* instruction counter starts at 100 per spec */
        int DC = 0;   /* data counter starts at 0 */
        Macro *mcro_head = NULL;

        name_length = (int)strlen(argv[i]);
        if ((name_length < 3) || strcmp(argv[i] + name_length -3, ".as") !=0) { /* check suffix .as */
            printf("Error: Filename must end with  '.as' \n");
            continue;
        }
        if (name_length >= (int)sizeof(src)) {
            printf("Error: filename must not exceed 255 chars\n");
            continue;
        }
        sprintf(src, "%s", argv[i]);
        sprintf(am,  "%s", argv[i]);  /* am  = "prog.am" */
        am[name_length -1] = 'm'; /* change to .as -> .am*/
        if (macro_expand(src, am, &mcro_head) == 0){ /* send src and pointer to dst */
            continue;
        }

    if (first_pass(am,mcro_head,&IC,&DC, inst_image, data_image, &symbols) == 0){
        free_macros(mcro_head);
        printf("Error:first pass errors occured\n");
        continue; /* got errors in first pass, continue to next file*/
    }
        
    free_macros(mcro_head);
    free_ext_int();
    }
   /*

 line_type type = classify_line(line){

       switch (type) 
               case LINE_EMPTY:
               case LINE_COMMENT:
                   break;
               case LINE_INSTRUCTION:
                   IC += 4;
                   break;
               case LINE_DIRECTIVE:
                   DC += parse_directive(line);
                   break;
                
                    return 0;

           }
     
       */


}
