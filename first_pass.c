#include "datatypes.h"
#include "classifier_helper.h"
#include "classifier.h"
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>


int first_pass(char *am,Macro *mcro_head) {
int line_num=0;
char line[MAX_LINE];
line_type type;
FILE *in;
int has_label;
char *p;
char label_name[MAX_NAME];
int error_flag =0;
Symbol *symbol_head = NULL;

/* open am file for reading after macro expension*/
in = fopen(am,"r");
if (!in){
    printf("Error: cannot open %s\n", am); 
    return -1;

}
while (fgets(line,MAX_LINE,in) != NULL){
    line_num ++;
    p=line;
    has_label =is_label(&p, label_name, mcro_head);
    if (has_label == -1){
        printf("Error: Invalid label name %d \n", line_num);
        error_flag ++;
        continue;
    }
    
    type = classify_line(p); 
        switch (type) {/* check for a lable */

               case LINE_EMPTY:
               case LINE_COMMENT:
                   continue;

               case LINE_INSTRUCTION:
               if (!has_label){

               } else {} 
               /*encode add to instruction image */
                   continue;
               case LINE_DIRECTIVE:
                if (!has_label){
                
               } else {}
                   /* is it data or entry/extern?*/
                   break;
                }
}
fclose(in);
if (error_flag) return -1;
else {
    return 0;

}

}