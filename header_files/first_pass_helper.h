#ifndef CLASSIFIER_HELPER_H
#define CLASSIFIER_HELPER_H

int add_symbol(Symbol_table *table,char *name, int value, int code, int dat, int ent, int ext);
int valid_label(char *name, Macro *mcro);
int find_symbol(Symbol_table *symbols, char *name);
int get_operand(char *p, char *op);
const Operation *find_op(char **line);
int check_end(char **p, int line_num);
int skip_whitespaces(char **p);
int is_comma(char **p, int line_num);
int read_label(char **p, char *label,int line_num, Macro *mcro_head);
int read_imd (char **p, int *num, int line_num);
int read_reg(char **p, int *reg, int line_num);
int parse_three_regs(char *p, int *rs, int *rt, int *rd, int line_num);
int parse_two_regs(char *p, int *rs, int *rt, int line_num);
int parse_reg_number_reg(char *p, int *rs, int *imd, int *rt, int line_num );
int parse_two_regs_label(char *p, int *rs, char *label,int *rt, int line_num, Macro *macro_head );
int parse_label_or_reg(char *p, int *rs, char *label,int *reg_flag, int line_num, Macro *macro_head);
int parse_label(char *p, char *label, int line_num, Macro *macro_head);
int validate_hlt(char *p,int line_num);
unsigned int encoder_j(int opcode, int reg_flag, int address);
unsigned int encoder_i(int opcode, int rs, int rt, int immed);
unsigned int encoder_r(int opcode,int rs,int rt,int rd, int funct);
#endif

