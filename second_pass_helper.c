#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "second_pass.h"
#include "datatypes.h"
#include "constants.h"
#include "classifier.h"
#include "first_pass_helper.h"

/*
* Adds an external symbol use to the external symbol table
* Stores the symbol name and the instruction address
* Returns 1 when successful and 0 when memory allocation fails
*/
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

/*
* Releases every node in the external symbol table
* Resets the table to an empty state
*/
void free_externs(Extern_table *externs) {
    Extern_symbol *curr;
    Extern_symbol *next; 

    curr = externs ->head; /* set curr to be the head of table */

    while (curr != NULL) { /*as long as the list is not empty set its next to be the next then free (curr) then move on to the next */
        next = curr ->next;
        free(curr);
        curr = next;
    }
    externs->head = NULL;
    externs->tail = NULL;
}
