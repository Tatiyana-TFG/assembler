#include <stdio.h>
#include <string.h>
#include "second_pass.h"
#include "datatypes.h"
#include "constants.h"
#include "classifier.h"
#include "first_pass_helper.h"
#include <stdlib.h>

  int add_ext_use(Extern_table *externs, char *name, int address) { 
    Extern_symbol *curr = malloc(sizeof(Extern_symbol));

        if (!curr){ 
            printf("Error: cannot allocate space for '%s',\n",name);
            return 0;
        } 
           
        strcpy(curr->name,name);
        curr->address=address;
        curr->next = NULL;

        if (externs->head == NULL)
            externs->head = externs->tail = curr; /* only node in the list both head and tail*/
        else {
            externs ->tail ->next = curr; /* more nodes exist go to the tail and set its next to be curr*/
            externs->tail = curr;/* then set the tail to point at the last added node*/
        }
        return 1;
    } 



Extern_symbol *find_extern(Extern_table *externs, char *name) { /*null if not found works for externs*/
        Extern_symbol *p = externs ->head;
        while (p != NULL){
            if (strcmp(p-> name,name)== 0)
                return p;
            p = p->next;
        }
        return NULL;
    } 
