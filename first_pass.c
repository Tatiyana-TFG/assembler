#include "datatypes.h"
#include "first_pass_helper.h"
#include "classifier.h"
#include <stdlib.h> 
#include <string.h>
#include <stdio.h>

int first_pass(char *am,Macro *mcro_head,int *IC,int *DC, int *inst_image, int *data_image, Symbol_table *symbols, Symbol_table *externs, Symbol_table *entry) {
int line_num=0;
char line[MAX_LINE];
line_type type;
directive_type dt;
FILE *in;
int has_label;
int rs,rt,rd,imd, address;
int reg_flag =0;
char *p;
char label[MAX_NAME];
char op[MAX_NAME];
char label_name[MAX_NAME];
int error_flag =0;
unsigned int encoded_val;
const Operation *operation;

/* open am file for reading after macro expension*/
in = fopen(am,"r");
if (!in){
    printf("Error: cannot open %s\n", am); 
    return -1;

}
while (fgets(line,MAX_LINE,in) != NULL){
    line_num ++;
    p=line;
    rs = rt = rd = address = reg_flag = imd = 0;
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
               if (has_label){ /*-1 case handled in line 32*/
                    if(add_symbol(symbols,label_name, *IC,1,0,0,0)==0){
                        error_flag++; continue; /* need to add error print?*/
                    }
               }
               operation = find_op(&p);
               if (operation == NULL){
                printf("Error: unknown operation in line %d\n",line_num);
                error_flag++;
                continue;
               }
                switch (operation->operands_type) {
                    case THREE_REG:
                    if (!parse_three_regs(p, &rs, &rt,&rd,line_num)){
                        error_flag ++;
                        continue;
                    }
                    break; /*finish inner switch and do whats after the switch ie encode*/
                    case TWO_REG:
                    if (!parse_two_regs(p, &rs, &rt,line_num)){
                        error_flag ++;
                        continue;
                    }
                    break; 

                    case REG_NUMBER_REG:
                    if (!parse_reg_number_reg(p, &rs, &imd, &rt,line_num)){
                        error_flag ++;
                        continue;
                    }
                    break;

                    case TWO_REGS_LABEL:
                        if (!parse_two_regs_label(p, &rs, label, &rt, line_num ,mcro_head)){
                        error_flag ++;
                        continue;
                    }
                    break;

                    case LABEL_OR_REG:
                        if (!parse_label_or_reg(p, &address, label,&reg_flag,line_num, mcro_head)){
                        error_flag ++;
                        continue;
                    }
                    break;

                    case LABEL_ONLY:
                    if (!parse_label(p,label,line_num, mcro_head)){
                        error_flag ++;
                        continue;
                    }
                    break;

                    case NO_OPERANDS:
                    if (!validate_hlt(p,line_num)){
                        error_flag ++;
                        continue;
                    }
                    break;
                }

                switch (operation->format){
                    case 'R':
                        encoded_val = encoder_r(operation ->opcode, rs, rt, rd, operation->funct);
                        break;
                    case 'I':
                        encoded_val = encoder_i(operation ->opcode, rs, rt, imd);
                        break;
                    case 'J':    
                        encoded_val = encoder_j(operation ->opcode, reg_flag, address );
                        break;
                }
                store_value();

               case LINE_DIRECTIVE:
               dt = classify_dir(p);
               switch (dt){
                case EXTERN:
                if (get_operand(p,op)){
                    if(add_symbol(externs,op, 0,0,0,0,1)==0){ /* need to add error print?*/
                        error_flag++; continue;
                    } continue;
                }printf("Error: opernad name not leagl\n");
                error_flag++; continue;
            
                case ENTRY:
                if (get_operand(p,op)){
                    if(add_symbol(entry,op, 0,0,0,1,0)==0){
                        error_flag++; continue;
                    } continue;
                } printf("Error: opernad name not leagl\n");
                error_flag++;
                continue;

                case DATA:
                case STRING:
                if (has_label) {
                if (add_symbol(symbols, label_name, *DC, 0,1,0,0) == 0) {
                error_flag++; continue;
                }
            } /* encode the values into data_image[*DC ...]; *DC += count; */
            continue;         

            case BAD_DIR:
            printf("Error: unknown line directive in line %d\n", line_num);
            error_flag ++;
            continue;
            }/*dt switch*/     
    }/*type switch*/
}/*while loop*/
fclose(in);
if (error_flag) return -1;
return 0;
}/*funct*/

