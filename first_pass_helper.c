#include <stdlib.h> 
#include <string.h>
#include "first_pass_helper.h"
#include "datatypes.h"
#include "constants.h"
#include "macro_helpers.h"

/* Operation table used to identify instructions and their encoding format*/
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

/*
 * Finds an operation by its name
 * Advances the line pointer past the operation name
 * Returns the operation information or NULL
 */
const Operation *find_op(char **line){
    char *p = *line;
    int i;
    char *tok;
    int const op_elements = NUMBER_OF_OP_ELEMENTS; 
    char *line_end = p + strlen(p);
    while (*p== ' ' || *p== '\t') p++;
    tok = strtok(p,"\t \n"); /* Get the operation name */
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
    }   return NULL; /* Return NULL when the operation is unknown */

}

/*
 * Checks whether a label follows the naming rules
 * Rejects reserved words and macro names
 * Returns 1 for a valid label and 0 otherwise
 */
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

/*
 * Searches the symbol table for a matching name
 * Returns the matching symbol or NULL when it is not found
 */
Symbol *find_symbol(Symbol_table *symbols, char *name) { /*null if not found*/
        Symbol *p = symbols ->head;
        while (p != NULL){
            if (strcmp(p-> name,name)== 0)
                return p;
            p = p->next;
        }
        return NULL;
    } 

/*
 * Creates a new symbol and adds it to the symbol table
 * Stores its address and its symbol attributes
 * Returns 1 when successful and 0 on failure
 */
int add_symbol(Symbol_table *table,char *name, int value, int code, int dat, int ent, int ext, int line_num) { /*success added on 1*/
    if (find_symbol(table,name) == 0){
        Symbol *curr = malloc(sizeof(Symbol));
        if (!curr){ 
            printf("Error: cannot allocate space for '%s' in line %d,\n",name, line_num);
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
    } printf("Error: symbol '%s' in line %d already defined\n", name, line_num);
    return 0; /* 0 for not added */
}

/*
 * Declares a name as an external symbol
 * Allows repeated declarations of the same external symbol
 * Rejects names already used by regular symbols
 */
int declare_extern(Symbol_table *table, char *name, int line_num){
Symbol *found;
found = find_symbol (table, name);
    if (found == NULL) {
        return add_symbol(table,name,0,0,0,0,1, line_num);
    } else {
        if (found->is_ext == 0) {  /*check if name is an extern attribute */
            printf("Error: name '%s'  in line %d is already declared and not as an extern\n", name, line_num);
            return 0;
        } else{
            printf("Extern %s in line %d is already defined (no error)\n" , name, line_num);
            return 1;
        }
           
    } 

 }

/*
 * Reads one label operand from a directive
 * Validates the operand and copies it to the output buffer
 * Rejects missing operands and extra text
 */
int get_operand(char *p, char *op, int line_num, Macro *mcro_head) {
char *tok;
char *extra;

tok = strtok(p, " \t \n"); /*directive*/
tok = strtok(NULL," \t \n"); /*operand*/

if (!tok){
    printf("Error: missing operands in line %d\n", line_num);
    return 0;
}
if (strlen(tok) >= MAX_NAME) {
    printf("Error: operand is too long in line %d\n", line_num);
    return 0;
}
    
if (!valid_label(tok, mcro_head)) {
    printf("Error: invalid label operand in line %d\n", line_num);
    return 0;
}

extra = strtok(NULL, " \t\n");

if (extra != NULL) {
    printf("Error: extra text after operand in line %d\n", line_num);
    return 0;
}

strcpy(op, tok); /*copy for caller function*/
return 1;
}

/*
 * Checks that no unexpected characters remain in the line
 * Returns 1 when the line ends correctly and 0 otherwise
 */
int check_end (char **p, int line_num){
    if (**p != '\n' && **p != '\0' ){
        printf("Error: extra elements after the last register in line %d \n ", line_num);
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

/*
 * Checks that the current character is a comma
 * Advances the pointer when a comma is found
 * Returns 1 when successful and 0 otherwise
 */
int is_comma(char **p, int line_num){

    if (**p== ','){
        (*p)++;
        return 1;
    }
    printf ("Error in line %d a comma is missing ", line_num);
    return 0;
}

/*
 * Reads a register number from the current position
 * Accepts register numbers from 0 through 31
 * Stores the result in reg
 */
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

/*
 * Reads a signed decimal immediate value
 * Checks that the value fits in the signed 16 bit range
 * Stores the result in num
 */
int read_imd (char **p, int *num, int line_num){
    int sign = 1;
    long value = 0;
    int digit;

    if (**p == '-'){ /* sign exists go past it*/
        sign = -1;
        (*p)++;
    }
    else if (**p == '+') /* sign exists go past it*/
        (*p)++;

    if ( **p <'0' || **p >'9'){ /*check it is digit*/
        printf("Error: Invalid immed in line %d \n", line_num);
        return 0;
    }

    while (**p >= '0' && **p <= '9'){ /*loop through conver char to int */
        digit = **p - '0'; 
        value = value * 10L + digit; /* add each digit to curr value*/

        if (sign == -1) { /* check that with each added digit we dont get out of range */
            if (value > (long)IMMEDIATE +1L) { /* if value is bigger than 32768 it will be out of range for the immed min*/
                printf("Error: immed in line %d is out of range \n", line_num);
                return 0;
            }
        }else {
            if (value > (long)IMMEDIATE) {
                printf("Error: immed in line %d is out of range \n", line_num);
                return 0;
            }
        }

        (*p)++; 
    }

    value *= sign; /*apply sign*/

    if (value < -((long)IMMEDIATE +1) || value > IMMEDIATE) { /*bounds between -32768 to 32767*/
        printf("Error: immed is out of range in line %d\n", line_num);
        return 0;
    }

    *num = value;
    return 1;

}

/*
 * Reads one label operand from the current position
 * Validates the label name and stores it in label
 * Rejects invalid names and unexpected extra text
 */
int read_label(char **p, char *label,int line_num, Macro *mcro_head){/* Read a label without a colon */
int len;
char *start =  *p; /*point to the same place line points to*/
char *pp= *p; /*point to the same place line points to*/

/* 1 means valid label
0 means invalid label */

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

/*
 * Parses three register operands
 * Checks the required commas and the end of the line
 * Stores the registers in rs rt and rd
 */
int parse_three_regs(char *p, int *rs, int *rt, int *rd, int line_num){
/* Read each register and check the separators */
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

/*
 * Parses two register operands
 * Checks the required comma and the end of the line
 * Stores the registers in rs and rd
 */
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

/*
 * Parses a register an immediate value and a register
 * Stores the results in rs imd and rt
 * Checks operand separators and the end of the line
 */
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

/*
 * Parses two registers followed by a label
 * Stores the registers and label in the supplied output values
 * Checks operand separators and the end of the line
 */
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

/*
 * Parses an operand that may be a register or a label
 * Sets reg_flag when the operand is a register
 * Stores the parsed value in the matching output variable
 */
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

/*
 * Parses one label operand
 * Validates the label and checks that no extra text remains
 */
int parse_label(char *p, char *label, int line_num, Macro *macro_head){
skip_whitespaces(&p);
if (!read_label(&p,label,line_num, macro_head))
    return 0;
skip_whitespaces(&p);
if (!check_end(&p, line_num))
    return 0;
return 1;
}

/*
 * Checks that the hlt instruction has no operands
 * Returns 1 when the line ends after the instruction
 */
int validate_hlt(char *p,int line_num){
skip_whitespaces(&p);
return check_end(&p,line_num);
}

/*
 * Parses a data directive and its numeric operands
 * Validates the values according to the directive size
 * Encodes the values into the data image
 */
int parse_encode_data(char *p, int line_num, unsigned char *data_image, int *DC){ 
long *data_op_array;
int data_array_count = 0;
char *line_end;
char *token;
int success;

skip_whitespaces(&p);
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

/* Validate directive values and encode them in the data image */
if (strcmp(token, ".db")==0){
    data_op_array = is_valid_op_loop(p,line_num,1 , &data_array_count);
    if (data_op_array == NULL){
        return 0;
    } else{ /*encode per db = 1 byte */
        success = encode_data(data_op_array, data_array_count, 1, data_image, DC, line_num);
        free(data_op_array);
        return success;
    }
}

if (strcmp(token, ".dh")==0){
    data_op_array =  is_valid_op_loop(p,line_num,2 , &data_array_count);
    if (data_op_array == NULL){
        return 0;
    } else{ /*encode per db = 1 byte */
        success = encode_data(data_op_array, data_array_count, 2, data_image, DC,line_num);
        free(data_op_array);
        return success;
    }
}

if (strcmp(token, ".dw")==0){
    data_op_array =  is_valid_op_loop(p,line_num,4 , &data_array_count);
   if (data_op_array == NULL){
        return 0;
    } else{ /*encode per db = 1 byte */
        success = encode_data(data_op_array, data_array_count, 4, data_image, DC,line_num);
        free(data_op_array);
        return success;
    }
}

printf("Error: unknown data directive in line %d\n", line_num);
return 0;

}

/*
 * Checks whether a value fits in the requested number of bytes
 * Returns 1 for a valid value and 0 otherwise
 */
int is_valid_data_value(long value, int num_of_bytes){ /* checks value between signed minimum and unsigned max for each data*/

    if (num_of_bytes ==1)
        return value >= DB_MIN && value <= DB_MAX;        

    if (num_of_bytes ==2)
        return value >= DH_MIN && value <= DH_MAX;

    if (num_of_bytes ==4) 
        return value >= DW_MIN && value <= DW_MAX;

    return 0;
    
}

/*
 * Reads all numeric operands of a data directive
 * Validates the values and stores them in a dynamic array
 * Returns the array or NULL when an error occurs
 */
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
    temp = realloc (data_array, (*data_array_count +1 ) * sizeof(long)); /* Resize the value array */

    if (temp == NULL){
        printf("Error: cannot allocate memory in line %d\n",line_num); /*fail*/
        free(data_array);
        return NULL;
    }
    data_array = temp; /* Use the resized array */
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

/*
 * Parses an asciz string operand
 * Checks the quotation marks and printable ASCII characters
 * Stores the string and its terminating zero in the data image
 */
int parse_encode_string(char *p, int line_num, unsigned char *data_image, int *DC){ 
char *token, *line_end, *pp, *string_end, *string_start;
int  string_length;
pp = p;
line_end = pp+ strlen(pp);
token = strtok(pp, "\t \n");
if (token == NULL){
    printf("Error: Missing instruction/directive in line %d\n" , line_num);
    return 0;
}
pp = token + strlen(token);

if (pp < line_end)
    pp++;

skip_whitespaces(&pp);

if (*pp != '"'){
    printf("Error: missing opening quotes in line %d\n" , line_num);
    return 0;
}

pp++; /*if opening " exist - skip past it "*/
string_start = pp;

while (*pp != '"' && *pp != '\n' && *pp != '\0'){/* check all are printable asciz chars and skip them*/
    if ((unsigned char) *pp < 32 || (unsigned char) *pp > 126){      /* printable ASCII values*/
        printf("Error: string contains non printable ASCII chars in line %d\n", line_num);
        return 0;
    }
    pp++;
} 

if (*pp != '"'){ /* The closing quote is missing */
    printf("Error: missing closing quotes in line %d\n", line_num);
    return 0;
}

string_end = pp; /* mark as the end"*/
pp++; /*move past closing "quote"*/
skip_whitespaces(&pp);

if (*pp != '\0' && (*pp != '\n')){ /*check no extra chars after closing quotes*/
    printf("Error: extra chars after closing quotes are not allowed in line %d\n", line_num);
    return 0;
}

string_length = string_end - string_start;

/* check space for asciz in data image */
if(string_length + 1 > IMAGE_CAPACITY - (*DC)) {
    printf("Error: image capacity overflow in line %d \n", line_num);
    return 0;
}

/* Copy the string characters into the data image */
while (string_start < string_end ){
    data_image[*DC] = *string_start; /* insert char value to data image*/
    (*DC) ++; /*advance DC counter*/
    string_start++;
}
data_image[*DC] = '\0'; /* null term at the end of asciz*/
(*DC)++;

return 1;
}

/* Encoders place each instruction field in its specified bit position */
/*
 * Encodes an R instruction
 * Places the opcode registers and function code in their bit fields
 * Returns the encoded instruction word
 */
unsigned int encoder_r(int opcode,int rs,int rt,int rd, int funct){
    unsigned int inst_value =0;
    inst_value |= (unsigned)opcode << 26;
    inst_value |= (unsigned)rs << 21;
    inst_value |= (unsigned)rt << 16;
    inst_value |= (unsigned)rd << 11;
    inst_value |= (unsigned)funct << 6;

    return inst_value;
}

/*
 * Encodes an I instruction
 * Places the opcode registers and immediate value in their bit fields
 * Keeps only the 16 bits of the immediate field
 */
unsigned int encoder_i(int opcode, int rs, int rt, int immed){
    unsigned int inst_value =0;
    inst_value |= (unsigned)opcode << 26;
    inst_value |= (unsigned)rs << 21;
    inst_value |= (unsigned)rt << 16;
    inst_value |= (unsigned)immed & 0xFFFF; /*get only 16 bit out of immed*/

    return inst_value;
}

/*
 * Encodes a J instruction
 * Places the opcode register flag and address in their bit fields
 * Keeps only the 25 bits of the address field
 */
unsigned int encoder_j(int opcode, int reg_flag, int address){
    unsigned int inst_value =0;
    inst_value |= (unsigned)opcode << 26;
    inst_value |= (unsigned)reg_flag << 25;
    inst_value |= (unsigned)address & 0x1FFFFFF; /* get 25 bits out of address */

    return inst_value;
}

/*
 * Encodes validated data values into the data image
 * Writes each value one byte at a time in little endian order
 * Returns 0 when the image has insufficient capacity
 */
int encode_data(const long *data_op_array, int data_array_count, int num_of_bytes, unsigned char *data_image, int *DC, int line_num){ /* from MSB byte <- LSB to LSB byte <- MSB*/
    int byte_ind, i;
    unsigned long value;
    int required_bytes;

    required_bytes = data_array_count * num_of_bytes; /*image capacity check*/
    if (required_bytes > IMAGE_CAPACITY - (*DC)) {
        printf("Error: Data image capacity is full in line %d\n", line_num);
        return 0;
    }

    for (i = 0; i < data_array_count; i ++){ /* how many numbers we have? */
        value = (unsigned long)data_op_array[i];

        for (byte_ind = 0; byte_ind < num_of_bytes; byte_ind ++){ /* Write the least significant byte first for little endian storage */
            data_image[*DC] = (unsigned char) ((value >> (byte_ind *8)) & 0xFFUL); /* shift the byte to the right (each byte is shifter a diff amount) and mask the rightmost byte and insert into image*/
            (*DC) ++; /* advance to the next DC per byte each time since we read a byte at a time*/
        }
    }
    return 1;
}

/*
 * Stores an encoded instruction in the instruction image
 * Calculates the array index from the current instruction address
 * Advances the instruction counter by four bytes
 */
int store_ic_value(unsigned int encoded_val, int *IC, unsigned int inst_image[]){
    int index;
    index = (*IC - 100) / 4;
    if (index >= IMAGE_CAPACITY){
        return 0;
    }
    inst_image[((*IC)-100)/4] = encoded_val; /*put encoded value in element 1 in the image array (ic holds 100)ic = 100 ->100 - 100 /4 => index 0 -> ic == 8 -> 108 - 100 /4 = index 2*/
    (*IC)+=4;
    return 1;
}

/*
 * Adjusts data symbol addresses after the instruction image is complete
 * Places every data symbol after the final instruction address
 */
void move_data_symbols (Symbol_table *symbols, int ICF) {
    Symbol *curr;
    curr = symbols -> head;
    while (curr != NULL) { /*while not null*/
        if (curr -> is_dat) { /* check each sym if its data*/
            curr -> value += ICF; /* if it is advance value by last IC counter value which is ICF*/
        }
        curr = curr ->next; /* inside while -> move to next sym*/
    }
}

/*
 * Releases all nodes in the symbol table
 * Resets the table to an empty state
 */
void free_symbols(Symbol_table *symbols) {
    Symbol *curr;
    Symbol *next; 

    curr = symbols ->head; /* set curr to be the list of table */

    while (curr != NULL) { /*as long as the head is not empty set its next to be the next then free (curr) then move on to the next */
        next = curr ->next;
        free(curr);
        curr = next;

    }
    symbols->head = NULL;
    symbols->tail = NULL;
}
