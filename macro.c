#include <stdio.h>
#include <string.h>

#define MAX_LINE   83 
#define MAX_MACROS 50
#define MAX_ROW   50

typedef struct {
    char name[32];
    char body[MAX_ROW][MAX_LINE];
    int  line_count;
} Macro;

static const char * forbiden_names[] = { 
    "add", "sub", "and", "or", "nor",
      "move", "mvhi", "mvlo",
      "addi", "subi", "andi", "ori", "nori",
      "bne", "beq", "blt", "bgt",
      "lb", "sb", "lw", "sw", "lh", "sh",
      "jmp", "la", "call", "hlt",
      ".db", ".dw", ".dh", ".asciz", ".entry", ".extern",
      "mcro", "mcroend",
      NULL};

static Macro table[MAX_MACROS];
static int   macro_count = 0; /* why at the top and not in function?*/

int line_too_long(const char *line, FILE *fp) {
    int c;
    if (strchr(line, '\n') == NULL && !feof(fp)) 
    {
        while ((c = fgetc(fp)) != '\n' && c != EOF);
        return 1;
    }
    return 0;
}
   
int find_macro (char *name)
{
    for (int i =0; i< macro_count; i ++) {
    if (strcmp(table[i].name, name)==0) {
        return 1;
        }
    }   
return 0;
}

int is_reserved(char *name) 
{
const char **p;

    for (p = forbiden_names ; *p != NULL; p++) { /*check if macro name is in the forbidden name array*/ 
                if (strcmp(*p, name) ==0){ /*if is then error and stop */
                return 1;
                }
            }
            return 0;
}

int over_macro_bound() {
    if (macro_count >= MAX_MACROS) 
      return 1;
    else
        return 0;
}

int macro_expand(const char *src, const char *dst) {
    char tmp[MAX_LINE];
    char *token;
    int line_num = 0;
    char line[MAX_LINE];                 /* buffer for one line from the source file */
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
                printf("Error: macro name is missing/n");
                fclose(in);
                fclose(out);
                return -1;
            }
            
            if (is_reserved(token) == 1)  { /* check name is reserved and illegal*/
                printf("Error: macro name is illeagl/n");
                fclose(in);
                fclose(out);
                return -1;
            }
            
            if (find_macro(token) == 1) { /*check exists in macro table already*/ 
                printf("Error: Macro name already exists\n");
                fclose(in);
                fclose(out);
                return -1;
            }
            
            if (strtok(NULL, " \t\n") != NULL) {  /*check nothing after name*/ 
                printf ("Error: in mcro definition extra chars are not legal\n"); 
                fclose(in);
                fclose(out);
                return -1;
            
            } else {

            if (over_macro_bound()==1) {
                printf("Error: too many macros, no space\n");
                fclose(in);
                fclose(out);
                return -1;
            }
            strcpy (table[macro_count].name, token);            /* add name to table */

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
                        return -1;
                    }
                    break; /* macroend termination */               
                } else { 
                    if (over_line_bounds()) {
                        printf("Error: ")
                    }

                }
            }
            macro_count++; /*proceed to next macro*/        
            }

        } else {
            if (is_token_macro){
                if (find_macro()){
                    expand_macro();
                }
            } else { 
            fputs(line, out);
            }
        }
    
    }

    fclose(in);
    fclose(out);
    return 0;
}