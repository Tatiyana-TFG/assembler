#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"

//helpers
static char *skip_ws (char *s)
{
      while (*s == ' ' || *s == '\t') s++;
        return s;
}
static int read_var (char **s) {
int idx;
    *s = skip_ws(*s);

    if (**s == ',') {
        printf("Illegal comma\n");
        return -1;
    }
    if (**s == '\n' || **s == '\0') {
        printf("Missing parameter\n");
        return -1;
    }

    if (**s < 'A' || **s > 'F') {
        printf("Undefined complex variable\n");
        return -1;
    }
    idx = **s - 'A'; // get the index itself 
    (*s)++; //skip the index once found
    return idx;
}

static int read_val (char **s, double *val ){
char *end;
    *s = skip_ws(*s);

    if (**s == ',') {
        printf("Illegal comma\n");
        return -1;
    }
    if (**s == '\n' || **s == '\0') {
        printf("Missing parameter\n");
        return -1;
    }
    *val = strtod(*s, &end);

    if (end == *s){
        printf("Invalid parameter - not a number\n"); //didnt find a val bc pointer indicates since it stayed the same
        return -1;
    }
    
    *s = end; //return pointer to end after val found
    return 0;

}

static int read_comma (char **s){
    *s = skip_ws(*s);
    if (**s != ','){
        printf("Missing comma\n");
        return -1;    
    }
    (*s) ++; //skip the comma if found 
    *s = skip_ws(*s);

    if (**s == ','){ 
        printf("Multiple consecutive commas\n");
        return -1;    
    }
    return 0;
}

static int check_end (char *s) {
    s = skip_ws(s);
    if (*s != '\0' && *s != '\n' ) {
        printf("Extraneous text after end of command\n");
        return -1;
    }  
return 0;
}

int parse_var(char *rest){
int idx;
    idx = read_var (&rest);
    if (idx < 0) //error message comes from read_var but termination comes from prase_var
        return -1;

    if (check_end(rest)< 0)
        return -1;

    return idx;
}

int parse_two_vars(char *rest,int *idx1, int *idx2) {
    *idx1= read_var(&rest);
    if (*idx1 <0) 
        return -1;

    if (read_comma(&rest) < 0)
        return -1;

    *idx2 = read_var(&rest);

    if (*idx2 <0)
        return -1;

   if (check_end(rest) < 0)
        return -1;
    return 0;
}

int parse_var_and_double(char *rest, int *var, double *val) {
    *var= read_var(&rest);
    if (*var < 0)
        return -1;

    if (read_comma(&rest) < 0)
        return -1;

    
    if (read_val(&rest,val) <0)
        return -1;

    if (check_end(rest) < 0)
        return -1;

    return 0;

}

int parse_var_and_two_doubles(char *rest, int *var, double *val1, double *val2){

    *var = read_var(&rest); //error message comes from read_var termination comes from this function
    if (*var < 0)
        return -1;
    
    if (read_comma(&rest) < 0) 
        return -1;
    
    if (read_val(&rest,val1) <0)
        return -1;

    if (read_comma(&rest) < 0)
        return -1;

    if (read_val(&rest,val2) <0)
        return -1;

    if (check_end(rest) < 0)
        return -1;

    return 0;

}
