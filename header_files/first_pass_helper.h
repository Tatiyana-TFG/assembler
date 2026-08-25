#include <datatypes.h>
#ifndef CLASSIFIER_HELPER_H
#define CLASSIFIER_HELPER_H

int add_symbol(Symbol_table *table,char *name, int value, int code, int dat, int ent, int ext);
int valid_label(char *name, Macro *mcro);
Symbol *find_symbol(Symbol_table *symbols, char *name);
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
int parse_label_or_reg(char *p, int *address, char *label,int *reg_flag, int line_num, Macro *macro_head);
int parse_label(char *p, char *label, int line_num, Macro *macro_head);
int validate_hlt(char *p,int line_num);
unsigned int encoder_j(int opcode, int reg_flag, int address);
unsigned int encoder_i(int opcode, int rs, int rt, int immed);
unsigned int encoder_r(int opcode,int rs,int rt,int rd, int funct);
int encode_data(const long *data_op_array, int data_array_count, int num_of_bytes, unsigned char *data_image, int *DC);
int parse_encode_data(char *p, int line_num, unsigned char *data_image, int *DC);
int parse_string(char *p, int line_num, unsigned char *data_image, int *DC);
int is_valid_data_value(long value, int num_of_bytes);
long *is_valid_op_loop(char *p, int line_num, int num_of_bytes, int *data_array_count);
int store_ic_value(unsigned int encoded_val, int *IC, unsigned int inst_image[]);
int declare_extern(Symbol_table *table, char *name);
#endif
