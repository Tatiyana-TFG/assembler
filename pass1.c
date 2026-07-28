#include "datatypes.h"
#include "main.c"
#include "classifier.c"
#include "classifier_helper.h"
#include <stdlib.h> 
#include <string.h>

char line[MAX_LINE];

int first_pass(char *am) {
FILE *in;
/* open am file for reading*/
in = fopen(am,"r");
if (!in){
    printf("Error: cannot open %s\n", am);
    return -1;
    fclose(in);
}
while (fgets(line,MAX_LINE,in) != NULL){
    line_type type = classify_line(line); 
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
    }
}



}