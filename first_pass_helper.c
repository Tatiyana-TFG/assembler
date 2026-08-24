#include <stdlib.h> 
#include <string.h>
#include "datatypes.h"
#include "macro_helpers.h"
#define NUMBER_OF_OP_ELEMENTS 27
#define DB_MIN  (-128L)
#define DB_MAX  255L
#define DH_MIN  (-32768L)
#define DH_MAX  65535L
#define DW_MIN  (-2147483647L - 1L)
#define DW_MAX  4294967295L


static const Operation operation_info[] = {
/* R: register, register, register */
    {"add",   0,  1, 'R', THREE_REG},
    {"sub",   0,  2, 'R', THREE_REG},
    {"and",   0,  3, 'R', THREE_REG},
    {"or",    0,  4, 'R', THREE_REG},
    {"nor",   0,  5, 'R', THREE_REG},

    /* R: register, register */
    {"move",  1,  1, 'R', TWO_REG},
    {"mvhi",  1,  2, 'R', TWO_REG},
    {"mvlo",  1,  3, 'R', TWO_REG},

    /* I: register, number, register */
    {"addi", 10, -1, 'I', REG_NUMBER_REG},
    {"subi", 11, -1, 'I', REG_NUMBER_REG},
    {"andi", 12, -1, 'I', REG_NUMBER_REG},
    {"ori",  13, -1, 'I', REG_NUMBER_REG},
    {"nori", 14, -1, 'I', REG_NUMBER_REG},

    /* I: register, register, label */
    {"bne",  15, -1, 'I', TWO_REGS_LABEL},
    {"beq",  16, -1, 'I', TWO_REGS_LABEL},
    {"blt",  17, -1, 'I', TWO_REGS_LABEL},
    {"bgt",  18, -1, 'I', TWO_REGS_LABEL},

    /* I: register, number, register */
    {"lb",   19, -1, 'I', REG_NUMBER_REG},
    {"sb",   20, -1, 'I', REG_NUMBER_REG},
    {"lw",   21, -1, 'I', REG_NUMBER_REG},
    {"sw",   22, -1, 'I', REG_NUMBER_REG},
    {"lh",   23, -1, 'I', REG_NUMBER_REG},
    {"sh",   24, -1, 'I', REG_NUMBER_REG},

      /* J formats */
    {"jmp",  30, -1, 'J', LABEL_OR_REG},
    {"la",   31, -1, 'J', LABEL_ONLY},
    {"call", 32, -1, 'J', LABEL_ONLY},
    {"hlt",  63, -1, 'J', NO_OPERANDS}
  };

const Operation *find_op(char **line){
    char *p = *line;
    int i;
    char *tok;
    int const op_elements = NUMBER_OF_OP_ELEMENTS; 
    char *line_end = p + strlen(p);
    /* skip whitespaces*/
    while (*p== ' ' || *p== '\t') p++;
    tok = strtok(p,"\t \n"); /* get the operatation name into token and compare in while loop*/
    if (tok == NULL)
        return NULL;
    p = tok + strlen(tok); /* go past the operation name*/ 
    if (p < line_end) /*move past the \0 added by the strtok only if delimiter exists*/
        p++;  
    *line = p;/*move pointer from caller function*/
    for (i=0; i < op_elements; i++){
        if (strcmp(tok,operation_info[i].name)==0){ /* find the array element with the operation name*/
            return &operation_info[i]; /*returns the array element*/
        }
    }   return NULL; /* return null pointer if dosent exist*/

}

int valid_label(char *name, Macro *mcro){ /*1 valid -  0 invalid*/
char *p;
if (!((name[0] >= 'a' && name[0] <= 'z') || (name[0]>= 'A' && name [0]<='Z')))
    return 0;
for (p = name +1 ; *p != '\0'; p++){
    if (!((*p >= 'a' && *p  <= 'z') || (*p  >= 'A' && *p  <= 'Z') || (*p >= '0' && *p <= '9'))) /* check all is  or num letters*/
        return 0;
}
if (!is_reserved(name)){
    if(!find_macro(mcro,name)){
        return 1;
        } return 0;
    } return 0;
}

int find_symbol(Symbol_table *symbols, char *name) {/* foud on 1*/
        Symbol *p = symbols ->head;
        while (p != NULL){
            if (strcmp(p-> name,name)== 0)
                return 1;
            p = p->next;
        }
        return 0;
    } 

int add_symbol(Symbol_table *table,char *name, int value, int code, int dat, int ent, int ext) { /*success added on 1*/
    if (find_symbol(table,name) == 0){
        Symbol *curr = malloc(sizeof(Symbol));
        if (!curr){ 
            printf("Error: cannot allocate space for '%s',\n",name);
            return 0;
        } 
           
        strcpy(curr->name,name);
        curr->value=value;
        curr->is_code = code;
        curr->is_dat = dat;
        curr->is_ent = ent;
        curr->is_ext = ext;
        curr->next = NULL;
        if (table->head == NULL)
            table->head = table->tail = curr; /* only node in the list both head and tail*/
        else {
            table ->tail ->next = curr; /* more nodes exist go to the tail and set its next to be curr*/
            table->tail = curr;/* then set the tail to point at the last added node*/
        }
        return 1;
    } printf("Error: symbol '%s' already defined\n", name);
    return 0; /* 0 for not added */
}

int add_extern(); /*ADD THIS*/

int get_operand(char *p, char *op) {
char *tok;
strtok(p, " \t \n");
tok = strtok(NULL," \t \n");
if (!tok) return 0;
strcpy(op,tok);
return 1;
}

int check_end (char **p, int line_num){
    if (**p != '\n' && **p != '\0' ){
        printf("Error: execess elemnts after last register in line %d \n ", line_num);
        return 0;
    }
    return 1;
}

int skip_whitespaces(char **p) {
    while (**p== ' ' || **p== '\t'){
        (*p)++;
    }
    return 1;
}

int is_comma(char **p, int line_num){

    if (**p== ','){
        (*p)++;
        return 1;
    }
    printf ("Error in line %d a comma is missing ", line_num);
    return 0;
}

int read_reg(char **p, int *reg, int line_num){
int value = 0;
    if (**p != '$'){ /* check symbol is $ */
        printf ("Error in register name '$' is needed in line %d", line_num);
            return 0;
    }
    (*p)++;
    if (**p < '0' || **p > '9'){ /* check after symbol there is digit*/
        printf ("Error in register name after '$' a number is needed in line %d", line_num);
        return 0; 
    }
           
    value = (**p - '0'); /* there is one digit*/
    (*p)++; 
    if (**p >= '0' && **p <= '9'){ /*if second digit exists under these conditions add the single digit to the value of first digit mult by *10*/
        value = value * 10 + (**p - '0'); /* get the nubmer by deducting ascii value of 0 from number*/
        (*p)++; /*move pointer past the digit*/
    }
    if (value > 31) {
        printf ("Register number can't be bigger than 31 %d \n " , line_num); 
        return 0;
    }

    if (**p >= '0' && **p <= '9'){
        printf ("Register number can't be bigger than 31 %d \n ", line_num); 
        return 0;  
    }
    *reg = value; /*keep value of register and pass back to caller*/
            
    return 1;

}

int read_imd (char **p, int *num, int line_num){
    int sign = 1;
    int value = 0;

    if (**p == '-'){ /* sign exists go past it*/
        sign = -1;
        (*p)++;
    }
    if (**p == '+') /* sign exists go past it*/
        (*p)++;

    if ( **p <'0' || **p >'9'){ /*check num is digit*/
        printf("Error: Invalid immed in line %d \n", line_num);
        return 0;
    }

    while (**p >= '0' && **p <= '9'){
        value = value *10 + (**p - '0'); /* get the int number*/
        (*p)++;
    }

    value *= sign; /*apply sign*/

    if (value < -32768 || value > 32767)
        return 0; /*check 16 bit range*/

    *num = value;
    return 1;

}

int read_label(char **p, char *label,int line_num, Macro *mcro_head){/* check if it is a lable name without :*/
int len;
char *start =  *p; /*point to the same place line points to*/
char *pp= *p; /*point to the same place line points to*/

/* 1 = valid lable 
0 = bad lable */

while (*pp != '\0' && *pp != '\t' && *pp != '\n' && *pp != ' ') pp++; /* find first token*/
len = pp - start; /* counter of how many chars the token is */
if (len == 0 || len >= MAX_NAME) { /* check length*/
    printf("Error: Invalid label name in line %d \n", line_num);
    return 0; /* invalid length*/
}
memcpy(label,start,len); /*copy to name from start at length of len*/
label [len] = '\0'; /*add terminator \0 at the end*/

if (label[len -  1] == ':'){ /*check no : after name*/
    printf("Error: label name in line %d should not contain ':' after definition\n", line_num);
    return 0;
}

if (!valid_label(label,mcro_head)){ /* check name validity*/
    printf("Error: invalid label name in line %d \n", line_num);
    return 0;
} 
skip_whitespaces(&pp);
if (!check_end(&pp, line_num))
    return 0;
*p=pp; /* advance caller pointer to past label */
return 1;
}

int parse_three_regs(char *p, int *rs, int *rt, int *rd, int line_num){
/*skip whitespaces -> check reg (including $ and valid number) -> skipws before and after comma -> check comma -> read register ...*/
skip_whitespaces(&p);
if (!read_reg(&p,rs,line_num))
    return 0;
skip_whitespaces(&p);
if (!is_comma(&p,line_num))
    return 0;
skip_whitespaces(&p);
if (!read_reg(&p,rt,line_num))
    return 0;
skip_whitespaces(&p);
if (!is_comma(&p,line_num))
    return 0;
skip_whitespaces(&p);
if (!read_reg(&p,rd,line_num))
    return 0;
skip_whitespaces(&p);
if (!check_end(&p, line_num))
    return 0;
return 1;
}

int parse_two_regs(char *p, int *rs, int *rd, int line_num){
skip_whitespaces(&p);
if (!read_reg(&p,rs,line_num))
    return 0;
skip_whitespaces(&p);
if (!is_comma(&p,line_num))
    return 0;
skip_whitespaces(&p);
if (!read_reg(&p,rd,line_num))
    return 0;
skip_whitespaces(&p);
if (!check_end(&p, line_num))
    return 0;
return 1;
}

int parse_reg_number_reg(char *p, int *rs, int *imd, int *rt, int line_num ){
skip_whitespaces(&p);
if (!read_reg(&p,rs,line_num))
    return 0;
skip_whitespaces(&p);
if (!is_comma(&p,line_num))
    return 0;
skip_whitespaces(&p);
if (!read_imd(&p, imd, line_num))
    return 0;
skip_whitespaces(&p);
if (!is_comma(&p,line_num))
    return 0;
skip_whitespaces(&p);
if (!read_reg(&p,rt,line_num))
    return 0;
skip_whitespaces(&p);
if (!check_end(&p, line_num))
    return 0;
return 1;
}

int parse_two_regs_label(char *p, int *rs, char *label, int *rt, int line_num, Macro *macro_head ){
skip_whitespaces(&p);
if (!read_reg(&p,rs,line_num))
    return 0;
skip_whitespaces(&p);
if (!is_comma(&p,line_num))
    return 0;
skip_whitespaces(&p);
if (!read_reg(&p,rt,line_num))
    return 0;
skip_whitespaces(&p);
if (!is_comma(&p,line_num))
    return 0;
skip_whitespaces(&p);
if (!read_label(&p, label, line_num, macro_head))
    return 0;
skip_whitespaces(&p);
if (!check_end(&p, line_num))
    return 0;
return 1;
}

int parse_label_or_reg(char *p, int *address_or_reg, char *label,int *reg_flag, int line_num, Macro *macro_head){
skip_whitespaces(&p);
if (*p == '$') { /* should there be a register?*/
    if (!(read_reg(&p,address_or_reg,line_num))){ /*is reg valid?*/
        return 0;
    }
    *reg_flag = 1; /* flag register */
} else {
    *reg_flag = 0;
    *address_or_reg = 0; /*placeholder until 2nd pass*/

    if(!(read_label(&p,label,line_num, macro_head)))
        return 0;

    }

skip_whitespaces(&p);
if (!check_end(&p, line_num))
    return 0;
return 1;

}

int parse_label(char *p, char *label, int line_num, Macro *macro_head){
skip_whitespaces(&p);
if (!read_label(&p,label,line_num, macro_head))
    return 0;
skip_whitespaces(&p);
if (!check_end(&p, line_num))
    return 0;
return 1;
}

int validate_hlt(char *p,int line_num){
skip_whitespaces(&p);
return check_end(&p,line_num);
}

int parse_encode_data(char *p, int line_num, unsigned char *data_image, int *DC){ 
long *data_op_array;
int data_array_count = 0;
char *line_end;
char *token;

skip_whitespaces(&p); /* skip whitespaces*/
line_end = p + strlen(p); /* point to last \0*/
token = strtok(p, "\t \n"); 
if (token == NULL) {
    printf("Error: missing data directive in line %d\n",line_num);
        return 0;
}
p = token + strlen(token); /* update p to point on last char \0*/

if (p < line_end){
    p++; /* if its not the last or only token then go past the  \0 created by strtok*/
}
skip_whitespaces(&p);

if (*p == '\0' || *p == '\n') {
    printf("Error: expected at least one number in line %d\n",line_num);
    return 0;
  }

/*function checks directive name -> calls op_loop -> op loop reads num and validates -> allocates mem and returns*/
if (strcmp(token, ".db")==0){
    data_op_array = is_valid_op_loop(p,line_num,1 , &data_array_count);
    if (data_op_array == NULL){
        return 0;
    } else{ /*encode per db = 1 byte */
        encode_data(data_op_array, data_array_count, 1, data_image, DC);
        free(data_op_array);
        return 1;
    }
}

if (strcmp(token, ".dh")==0){
    data_op_array =  is_valid_op_loop(p,line_num,2 , &data_array_count);
    if (data_op_array == NULL){
        return 0;
    } else{ /*encode per db = 1 byte */
        encode_data(data_op_array, data_array_count, 2, data_image, DC);
        free(data_op_array);
        return 1;
    }
}

if (strcmp(token, ".dw")==0){
    data_op_array =  is_valid_op_loop(p,line_num,4 , &data_array_count);
   if (data_op_array == NULL){
        return 0;
    } else{ /*encode per db = 1 byte */
        encode_data(data_op_array, data_array_count, 4, data_image, DC);
        free(data_op_array);
        return 1;
    }
}

printf("Error: unknown data directive in line %d\n", line_num);
return 0;

}

int is_valid_data_value(long value, int num_of_bytes){ /* checks value between signed minimum and unsigned max for each data*/

    if (num_of_bytes ==1)
        return value >= DB_MIN && value <= DB_MAX;        

    if (num_of_bytes ==2)
        return value >= DH_MIN && value <= DH_MAX;

    if (num_of_bytes ==4) 
        return value >= DW_MIN && value <= DW_MAX;

    return 0;
    
}

long *is_valid_op_loop(char *p, int line_num, int num_of_bytes, int *data_array_count){ /* function walks the line as long as not the end and checks each number then stores it */
long value;
char *end;
long *data_array = NULL;
long *temp; /* temp for realloc*/

while (*p != '\0' && *p != '\n'){
    skip_whitespaces(&p);

    value = strtol(p, &end, 10); /* get first value*/
    if (end == p) {
        printf("Error: expected a numeric data value in line %d\n", line_num);
        free(data_array);
        return NULL;
    }

    if (!is_valid_data_value(value,num_of_bytes)){ /*check if value is 1/2/4 byte range*/
    printf("Error: data value is out of range in line %d\n", line_num);
    free(data_array);
    return NULL;
}
    temp = realloc (data_array, (*data_array_count +1 ) * sizeof(long)); /*realoc space for new number*/

    if (temp == NULL){
        printf("Error: cannot allocate memory in line %d\n",line_num); /*fail*/
        free(data_array);
        return NULL;
    }
    data_array = temp; /* put reall*/
    data_array[*data_array_count] = value; 
    (*data_array_count) ++ ;

    p = end; /* advance past first num*/
    skip_whitespaces(&p);

    if (*p == '\0' || *p == '\n') { /* end of line */ 
        break;
    }

    if (!is_comma(&p, line_num)){ /* if not check for comma*/
        free(data_array);
        return NULL;
    }
    
    skip_whitespaces(&p);

    if (*p == '\0' || *p == '\n') { /* if only comma it is invalid must have number after*/
        printf("Error: missing number after comma in line %d\n",line_num);
        free(data_array);
        return NULL;
  }

}    return data_array;

} 

int parse_string(char *p, int line_num, unsigned char *data_image, int *DC){ 
char *token, *line_end, *pp, *string_end, *string_start, *string_length;
pp = p;
line_end = pp+ strlen(pp);
token = strtok(pp, "\t \n");
if (token == NULL){
    printf("Error: Missing infirmation in line %d\n" , line_num);
    return 0;
}
pp = token + strlen(token);

if (pp < line_end)
    pp++;

skip_whitespaces(&pp);

if (*pp != '"'){
    printf("Error: extra chars after string in line %d\n" , line_num);
    return 0;
}

pp++; /*if opening " exist - skip past it "*/
string_start = pp;

while (*pp != '"' && *pp != '\n' && *pp != '\0') /*skip all chars*/
    pp++;

if (*pp != '"'){ /*closing char dosent exist*/
    printf("Error: missing closing quotes in line %d\n", line_num);
    return 0;
}

if (*pp != '\0' || (*pp != '\n') || (*pp != '\t')){ /*check no extra chars after closing quotes*/
    printf("Error: extra chars after closing quotes are not allowed in line %d\n", line_num);
    return 0;
}

string_end = pp;
string_length = string_end - string_start; /* dont need string length?*/

/* check space for asciz in data image 
*/

/* loop from string start length of string length and each char put into data image */
while (string_start < string_end ){
    data_image[*DC] = *string_start; /* insert char value to data image*/
    (*DC) ++; /*advance DC counter*/
    string_start++;
}
data_image[*DC] = '\0'; /* null term at the end of asciz*/
(*DC)++;

return 1;
}


/*all encoders work the same -> move bits to designated position by bitwise operatoes */
unsigned int encoder_r(int opcode,int rs,int rt,int rd, int funct){
    unsigned int inst_value =0;
    inst_value |= (unsigned)opcode << 26;
    inst_value |= (unsigned)rs << 21;
    inst_value |= (unsigned)rt << 16;
    inst_value |= (unsigned)rd << 11;
    inst_value |= (unsigned)funct << 6;

    return inst_value;
}

unsigned int encoder_i(int opcode, int rs, int rt, int immed){
    unsigned int inst_value =0;
    inst_value |= (unsigned)opcode << 26;
    inst_value |= (unsigned)rs << 21;
    inst_value |= (unsigned)rt << 16;
    inst_value |= (unsigned)immed & 0xFFFF; /*get only 16 bit out of immed*/

    return inst_value;
}

unsigned int encoder_j(int opcode, int reg_flag, int address){
    unsigned int inst_value =0;
    inst_value |= (unsigned)opcode << 26;
    inst_value |= (unsigned)reg_flag << 25;
    inst_value |= (unsigned)address & 0x1FFFFFF; /* get 25 bits out of address */

    return inst_value;
}

void encode_data(const long *data_op_array, int data_array_count, int num_of_bytes, unsigned char *data_image, int *DC){ /* from MSB byte <- LSB to LSB byte <- MSB*/
    int byte_ind, i;
    unsigned long value;
    for (i = 0; i < data_array_count; i ++){ /* how many numbers we have? */
        value = (unsigned long)data_op_array[i];

        for (byte_ind = 0; byte_ind < num_of_bytes; byte_ind ++){ /* what size are these numbers? iterate number of bytes and insert into data image in little endian*/
            data_image[*DC] = (unsigned char) ((value >> (byte_ind *8)) & 0xFFUL); /* shift the byte to the right (each byte is shifter a diff amount) and mask the rightmost byte and insert into image*/
            (*DC) ++; /* advance to the next DC per byte each time since we read a byte at a time*/
        }
    }
}

void store_ic_value(unsigned int encoded_val, int *IC, unsigned int inst_image[]){
    inst_image[((*IC)-100)/4] = encoded_val; /*put encoded value in element 1 in the image array (ic holds 100)ic = 100 ->100 - 100 /4 => index 0 -> ic == 8 -> 108 - 100 /4 = index 2*/
    (*IC)+=4;
}

/*
free_symbol(){};
*/

/*
 - Implement .asciz. ->
  - Correct .extern.
  - Defer or properly record .entry.
  - Stop DATA from falling through into STRING.
  - Add final IC to every data symbol address.
  - Add capacity checks.
  */