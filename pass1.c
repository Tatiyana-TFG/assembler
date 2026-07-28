#include "datatypes.h"
#include "classifier_helper.h"
#include "classifier.h"
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>


int first_pass(char *am) {
char line[MAX_LINE];
line_type type;
FILE *in;

/* open am file for reading*/
in = fopen(am,"r");
if (!in){
    printf("Error: cannot open %s\n", am);
    return -1;

}
while (fgets(line,MAX_LINE,in) != NULL){
    type = classify_line(line); 
        switch (type) {
               case LINE_EMPTY:
               case LINE_COMMENT:
                   continue;
               case LINE_INSTRUCTION:
                   /* add to instruction image*/
                   continue;
               case LINE_DIRECTIVE:
                   /* is it data or entry/extern?*/
                   break;
                }
}
fclose(in);
return 0;

}