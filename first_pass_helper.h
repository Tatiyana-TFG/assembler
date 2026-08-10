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
int read_reg(char **p, int *reg, int line_num);
int parse_three_regs(char *p, int *rs, int *rt, int *rd, int line_num);


#endif

