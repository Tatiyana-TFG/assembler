#include <stdio.h>
#include <string.h>
#include "datatypes.h"

int main(int argc, char *argv[]) {
    char src[256], am[256];
    int IC = 100; /* instruction counter starts at 100 per spec */
    int DC = 0;   /* data counter starts at 0 */

    if (argc < 2) {
        printf("Proper command is this: assembler <filename>\n");
        return 1;
    }

    sprintf(src, "%s.as", argv[1]);  /* src = "prog.as" */
    sprintf(am,  "%s.am", argv[1]);  /* am  = "prog.am" */
    macro_expand(src, am);
    
        /* TODO: classify line and dispatch */
        /* line_type type = classify_line(line);
           switch (type) {
               case LINE_EMPTY:
               case LINE_COMMENT:
                   break;
               case LINE_INSTRUCTION:
                   IC += 4;
                   break;
               case LINE_DIRECTIVE:
                   DC += parse_directive(line);
                   break;
           } */
    

    return 0;
}
