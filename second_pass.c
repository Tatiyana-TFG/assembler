
#include <stdio.h>
#include <string.h>
#include "datatypes.h"
#include "classifier.h"
#include "first_pass_helper.h"
#include "macro.h"

int second_pass(char *am, Macro *mcro_head, unsigned int *inst_image, Symbol_table *symbols) {
FILE *in;
char line[MAX_LINE];
char label_name[MAX_NAME];
char op [MAX_NAME];
char *p;
int line_num;
int curr_ic;
int has_label;
int error_flag;
line_type type;
directive_type dt;
Symbol *symbol;


line_num = 0;
curr_ic = 100;
error_flag = 0;

/*open file*/
in = fopen(am, "r");

if (in == NULL) {
    printf("Error: cannot open %s in second pass\n", am); 
    return 0;
}
while (fgets(line,MAX_LINE,in) != NULL){
    line_num ++;
    p=line;

    /*pass two only needs to move p since pass 1 check label already*/

    has_label =is_label(&p, label_name, mcro_head);
    if (has_label == -1){
        printf("Error: in label name in line %d \n", line_num);
        error_flag ++;
        continue;
    }

    type = classify_line(p);

    switch (type) {/* what kind of line? */

               case LINE_EMPTY:
               case LINE_COMMENT:
                continue;

               case LINE_INSTRUCTION:
                    /*encode j type;*/
                curr_ic +=4;
                continue;
               case LINE_DIRECTIVE:
               dt = classify_dir(p, line_num);
               switch (dt) {
                case EXTERN:
                case DATA:
                case STRING:
                    continue;
                case ENTRY:
                    if (get_operand(p,op, line_num,mcro_head) == 0) {
                        error_flag ++;
                        continue;
                        /*get operand prints error message*/
                    }
                    symbol = find_symbol(symbols, op); /* op holds the operands return from get operand after .entry or extern*/
                    if (symbol == NULL) {
                        printf("Error: entry symbol '%s' is not defined in line %d\n", op, line_num);
                        error_flag++;
                        continue;
                    } /*check if entry label is in sym table*/
                    if (symbol -> is_ext) {
                        printf("Error: symbol '%s' cannot be both entry and external in line %d\n",op,line_num);
                        error_flag++;
                        continue;
                    } else {
                        symbol -> is_ent =1;
                        continue;
                    }

                case BAD_DIR:
                    printf("Error: unknown line directive in line %d\n", line_num);
                    error_flag ++;
                    continue;
                } /*dt switch close*/
               
                
                continue;


}

}
fclose(in);
if (error_flag) 
    return 0;
return 1;

}