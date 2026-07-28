#include <stdio.h>
#include <string.h>
#include "macro.h"
#include "datatypes.h"
#include "macro_helpers.h"

int first_pass(char *am, Macro *mcro_head, int IC,int DC, int *inst_image, int *data_image);

int main(int argc, char *argv[]) {
    char src[256], am[256]; /* maybe should also be dynamic */
    int IC = 100; /* instruction counter starts at 100 per spec */
    int DC = 0;   /* data counter starts at 0 */
    int i;
    int data_image[1000];
    int inst_image[1000];

    if (argc < 2) {
        printf("Proper command is this: assembler <filename>\n"); /* */
        return 1;
    } 
    
    for (i = 1; i < argc; i++) {
        Macro *mcro_head = NULL;
        sprintf(src, "%s.as", argv[i]);  /* src = "prog.as" */
        sprintf(am,  "%s.am", argv[i]);  /* am  = "prog.am" */
        if (macro_expand(src, am, &mcro_head) != 0){ /* send src and pointer to dst */
            continue;
        }
    if (first_pass(am,mcro_head,IC,DC, &inst_image, &data_image)!= 0){
        free_macros(mcro_head);
        printf("Error:first pass input errors occured\n");
        continue; /* got errors in first pass, continue to next file*/
    }
        
    free_macros(mcro_head);
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
