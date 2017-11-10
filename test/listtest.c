
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/linuxlist.h"

/*
 * Debug with gdb or ddd to set breakpoints and see what happens at each step.
 */

// Declaration of the actual data structure used in the linked list
struct testlist {
  int size;
  char text[64];
  struct list_head entry;
};

// Declare and initialize a linked list head
// There is no need for it to be an instance of struct testlist
LIST_HEAD(list);

int main(int argc, char **argv) {
  int i;
  char utext[64];
  struct testlist *new_entry;

  // Ask user to enter 5 strings, and add them to the linked list.
  printf("Enter 5 strings to be added to the linked list\n");
  for (i=0; i<5; i++) {
    scanf("%s", utext);
    // Create the new element and set values
    new_entry = malloc(sizeof(struct testlist));
    strncpy(new_entry->text, utext, 64);
    new_entry->size = strnlen(new_entry->text, 64);
    // Add the new element to the list
    list_add(&new_entry->entry, &list);
  }

  // Wait for user input to quit
  printf("Press any key to exit");
  getc(stdin); getc(stdin);

  // Free linked list
  while((new_entry = list_first_entry_or_null(&list, struct testlist, entry))) {
    list_del(&new_entry->entry);
    free(new_entry);
  }
  exit(EXIT_SUCCESS);
}
