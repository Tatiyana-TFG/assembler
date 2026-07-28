  #include <stdio.h>
  #include <string.h>
  #include "datatypes.h"
  #include "macro_helpers.h"
  #include "macro.h"

int macro_expand(const char *src, const char *dst, Macro **mcro_head) {
    char tmp[MAX_LINE];
    char *token;
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
        return -1;
    }
    /* open destination file for writing (creates it if it doesn't exist) */
    out = fopen(dst, "w");
    if (!out) 
    {
        printf("Error: cannot create %s\n", dst);
        fclose(in);                          /* must close in before returning — otherwise the file stays locked open */
        return -1;
    }


    while (fgets(line,MAX_LINE,in) != NULL)
    {
        strcpy(tmp,line);
        line_num ++;
        token = strtok(tmp, " \t\n");  /* get first token find the macros*/

        if (line_too_long(line,in)){
            printf("Error: Line %d too long\n" , line_num);
            continue;   
        }
            
        if (token && strcmp(token, "mcro") == 0){
            token = strtok(NULL, " \t\n"); /* token = name of macro call*/

            if (token == NULL) {
                printf("Error: macro name is missing\n");
                fclose(in);
                fclose(out);
                free_macros(mcro);
                return -1;
            }
            
            if (is_reserved(token) == 1)  { /* check name is reserved and illegal*/
                printf("Error: macro name is illeagl\n");
                fclose(in);
                fclose(out);
                free_macros(mcro);
                return -1;
            }
            
            if (find_macro(mcro,token) == 1) { /*check exists in macro table already*/ 
                printf("Error: Macro name already exists\n");
                fclose(in);
                fclose(out);
                free_macros(mcro);
                return -1;
            }
            
            if (strtok(NULL, " \t\n") != NULL) {  /*check nothing after name*/ 
                printf ("Error: in mcro definition extra chars are not legal\n"); 
                fclose(in);
                fclose(out);
                free_macros(mcro);
                return -1;
            
            } else {

            mcro= add_macro(mcro,token,&error);            /* add name to table */

            if (error){
                printf("Error: out of memory \n");
                fclose(in);
                fclose(out);
                free_macros(mcro);
                return -1;
            }  

            while (fgets(line,MAX_LINE,in)!= NULL) { /*add content to the table by line*/
                line_num ++;
                strcpy(tmp,line);
                token = strtok(tmp, " \t\n");

                if (line_too_long(line,in)) {
                        printf("Error: Line %d too long\n" , line_num);
                        continue;  
                }

                if (token && strcmp(token, "mcroend") == 0) {
                    if (strtok(NULL, " \t\n") != NULL) { /* if extra chars break with error*/
                        printf ("Error: in mcroend definition extra chars are not legal\n");
                        fclose(in);
                        fclose(out);
                        free_macros(mcro);
                        return -1;
                    }
                    break; /* macroend termination */               
                } else { 

                    add_line(mcro,line,&error);
                     if (error) {
                        printf("Error: out of memory\n");
                        fclose(in);
                        fclose(out);
                        free_macros(mcro);
                        return -1;
                    }

                }
            }
            }

        } else {
            /* loop the whole line against macro table */
                if (token && (find_macro(mcro,token))){
                    print_macro(mcro,out);
                } else { 
                fputs(line, out);
            }
        }
    
    }

    fclose(in);
    fclose(out);
    *mcro_head=mcro;
    return 0;
}