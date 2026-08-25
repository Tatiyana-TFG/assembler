
#include <stdio.h>
#include <string.h>
#include "second_pass.h"
#include "datatypes.h"
#include "constants.h"
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
int rs,rt, distance, index, inst_addr;
int curr_ic;
int has_label;
int error_flag;
int addr_or_reg;
int reg_flag;
int j_label;
line_type type;
directive_type dt;
Symbol *symbol;
const Operation *operation;


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
               addr_or_reg = 0;
               reg_flag = 0;
               j_label = 0;
                inst_addr = curr_ic;
                curr_ic +=4;
                operation = find_op(&p);
               if (operation == NULL){ /* to advance p past operation name */
                printf("Error: unknown operation in line %d\n",line_num);
                error_flag++;
                continue;
               }
               /* branching I operation */
                if (operation -> operands_type == TWO_REGS_LABEL) { /* will handel branch operands here */
                    if (!parse_two_regs_label (p,&rs, label_name, &rt, line_num,mcro_head)){
                        error_flag++;
                        continue;
                    }
                    symbol = find_symbol(symbols, label_name); /*label name here is the second label name and not going to get LABEL: definition*/
                    if (symbol == NULL) {
                        printf("Error: branch target %s is not defined in line %d\n", label_name, line_num);
                        error_flag++;
                        continue;
                    }
                    if (symbol ->is_ext) {
                        printf("Error: branch taget %s cannot be external in line %d\n", label_name, line_num);
                        error_flag++;
                        continue;
                    }
                    distance = symbol->value -inst_addr; /* find the distnace the action needs to use*/
                    
                    if (distance < BRANCH_DISTANCE_MIN || distance > BRANCH_DISTANCE_MAX) {
                        printf("Error: branch distance is out of range in line %d\n", line_num); /*check bounds for distance*/
                        error_flag++;
                        continue;
                    }
                    index = ((inst_addr - 100)/4); /*find the indext for the inst to encode full instruction*/
                    inst_image[index] = encoder_i(operation ->opcode, rs, rt, distance);
                    continue;
                    }
                /* j operation*/
                if (operation -> operands_type == LABEL_ONLY){
                    if (!parse_label(p,label_name,line_num, mcro_head)){
                        error_flag ++;
                        continue;
                    }
                    j_label = 1;
                    continue;
                }

                /*label or reg*/
                if (operation -> operands_type == LABEL_OR_REG){
                    if (!parse_label_or_reg(p, &addr_or_reg, label_name,&reg_flag,line_num, mcro_head)){
                        error_flag ++;
                        continue;
                    }
                    /*if its jmp label (jmp register was encoded in pass1)*/
                    if (reg_flag == 0)
                        j_label = 1;
                }
                if (j_label){
                    symbol = find_symbol(symbol, label_name));
                }
                
              



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