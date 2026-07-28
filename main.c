#include <stdio.h>
#include <string.h>
#include "macro.h"
#include "datatypes.h"

int main(int argc, char *argv[]) {
    char src[256], am[256]; /* maybe should also be dynamic */
    int IC = 100; /* instruction counter starts at 100 per spec */
    int DC = 0;   /* data counter starts at 0 */
    int i; /* whats this now ? */

    if (argc < 2) {
        printf("Proper command is this: assembler <filename>\n"); /* */
        return 1;
    } 
    
    for (i = 1; i < argc; i++) {
        sprintf(src, "%s.as", argv[i]);  /* src = "prog.as" */
        sprintf(am,  "%s.am", argv[i]);  /* am  = "prog.am" */
        if (macro_expand(src, am) != 0){ /* send src and pointer to dst */
            continue;
        }
        
    }
    first_pass(am);
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
