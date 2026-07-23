#ifndef PARSER_H
#define PARSER_H

int parse_3reg();
int parse_2reg();
int parse_reg_imm_reg();
int parse_2reg_lbl();
int parse_reg_imm_reg();
int parse_lbl_reg();
int parse_lbl();
int parse_no();






int parse_two_vars(char *rest, int *idx1, int *idx2);
int parse_var_and_double(char *rest, int *idx1, double *val);
int parse_var_and_two_doubles(char *rest, int *idx1, double *val1, double *val2);

#endif