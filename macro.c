#include <stdio.h>
#include <string.h>
#include "datatypes.h"
#include "macro_helpers.h"
#include "macro.h"
/*
* Expands macro definitions and macro calls from src into the intermediate
* file dst Stores the resulting macro table in mcro_head
* Returns 1 on success and 0 if an input output or macro error occurs
*/

int macro_expand(const char *src, const char *dst, Macro **mcro_head) {
    Macro *found;
    char *extra;
    char tmp[MAX_LINE];
    char *token;
    int error_flag = 0;
    int line_num = 0;
    char line[MAX_LINE];
    Macro *mcro = NULL;
    int error =0;                 /* buffer for one line from the source file */
    FILE *in, *out;

/* open source file for reading */

    in = fopen(src, "r");               /* open the filename char arry src is pointing to */
    if (!in) 
    {
        printf("Error: cannot open %s\n", src);
        return 0;
    }
    /* open destination file for writing (creates it if it doesn't exist) */
    out = fopen(dst, "w");
    if (!out) 
    {
        printf("Error: cannot create %s\n", dst);
        fclose(in);                          /* must close in before returning — otherwise the file stays locked open */
        return 0;
    }


    while (fgets(line,MAX_LINE,in) != NULL) /* get lines and look for macro definitions*/
    {
        strcpy(tmp,line);
        line_num ++;
        token = strtok(tmp, " \t\n");  /* get first token */

        if (line_too_long(line,in)){ /* line length check*/
            printf("Error: Line %d too long\n" , line_num);
            error_flag++;
            continue;   
        }
            
        if (token && strcmp(token, "mcro") == 0){ /* if the token is macro*/
            token = strtok(NULL, " \t\n"); /* the next token = name of macro call*/

    
            if (token == NULL) { /* error if no macro name defined*/
                printf("Error: macro name is missing in line %d \n", line_num);
                fclose(in);
                fclose(out);
                free_macros(mcro);
                return 0;
            }

            if (strlen(token) >= MAX_NAME){ /*check macro name length*/
                printf("Error: Macro name in line %d is too long \n", line_num);
                fclose(in);
                fclose(out);
                free_macros(mcro);
                return 0;
            }

            if (is_reserved(token) == 1)  { /* check name is not reserved and illegal*/
                printf("Error: macro name in line %d can't be a reserved name\n", line_num);
                fclose(in);
                fclose(out);
                free_macros(mcro);
                return 0;
            }
            
            if (find_macro(mcro,token) != NULL) { /*check if exists in macro table already*/ 
                printf("Error: Macro name in line %d already exists\n" , line_num);
                fclose(in);
                fclose(out);
                free_macros(mcro);
                return 0;
            }
            
            if (strtok(NULL, " \t\n") != NULL) {  /*check nothing comes after name*/ 
                printf ("Error: in line %d extra chars are not legal after mcro definition \n", line_num); 
                fclose(in);
                fclose(out);
                free_macros(mcro);
                return 0;
            
            } else { /* passed all name checks (name can be any combination of letters symbols and numbers)*/

            mcro= add_macro(mcro,token,&error);            /* add name to table */

            if (error){ /*malloc failed*/
                printf("Error: out of memory in macro table \n");
                fclose(in);
                fclose(out);
                free_macros(mcro);
                return 0;
            }  

            while (fgets(line,MAX_LINE,in)!= NULL) { /*go over the line after mcro def and add cont to the table by rules below*/
                line_num ++; /* line counter for error msgs*/
                strcpy(tmp,line);
                token = strtok(tmp, " \t\n");

                if (line_too_long(line,in)) {
                        printf("Error: Line %d too long\n" , line_num);
                        error_flag ++;
                        continue;  
                }

                if (token && strcmp(token, "mcroend") == 0) { /* is the token mcroend?*/
                    if (strtok(NULL, " \t\n") != NULL) { /* if extra chars after mcroend break with error*/
                        printf ("Error: in line %d mcroend definition extra chars are not legal\n", line_num);
                        fclose(in);
                        fclose(out);
                        free_macros(mcro);
                        return 0;
                    }
                    break; /* macroend termination */               
                } else { 
                    add_line(mcro,line,&error); /*add line content to table*/
                     if (error) { /* if malloc cant allocate */
                        printf("Error: out of memory\n");
                        fclose(in);
                        fclose(out);
                        free_macros(mcro);
                        return 0;
                    }

                }
            }
        }

        } else {
            found = NULL; /* */
            if (token != NULL) {
                found = find_macro(mcro,token); /* if token tokenized the macro name see if it exists and retunr * to found*/
            }

            if (found != NULL) {
                extra = strtok (NULL, " \t\n"); /* if the token exists we want to check nothing else comes after the name*/
            

                if (extra != NULL) {
                    printf("Error: extra text after macro name '%s' in line %d\n", token, line_num);
                    error_flag ++;
                    continue;
                }
            
            print_macro(found,out); 
        } else {
            fputs(line, out);
            }
        }
    }

    

    fclose(in);
    fclose(out);
    
    if (error_flag) {
        free_macros(mcro);
        *mcro_head = NULL;
        remove(dst);
        return 0;
    }

    *mcro_head=mcro;
    return 1;
}
