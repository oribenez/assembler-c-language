#include "external_linked_list.h"
#include <stdio.h>

/* This function adds a node to the end of the list */
ext_ptr ext_insert_item(ext_ptr *hptr, char *name, unsigned int reference) {
    ext_ptr t, temp;

    t = *hptr;
    temp = (ext_ptr)malloc_w_check(sizeof(ext));

    temp->address = reference;
    strcpy(temp->name, name);

    /* If the list is empty */
    if (!(*hptr)) {
        *hptr = temp;
        temp->next = temp;
        temp->prev = temp;
        return temp;
    }

    ((*hptr)->prev)->next = temp;
    temp->next = t;
    temp->prev = t->prev;
    (*hptr)->prev = temp;

    return temp;
}

/* This function frees the allocated memory for the list */
void ext_free_list(ext_ptr *hptr) {
    /* Frees the extern list by going over each extern node and free it*/
    ext_ptr temp;
    unsigned int last_reference, reference;

    temp = *hptr;
    if (temp) {
        last_reference = ((*hptr)->prev)->address;
        reference = 0;
        do {
            temp = *hptr;
            reference = temp->address;
            *hptr = (*hptr)->next;
            free(temp);
        } while (reference != last_reference);
    }
}

/* This function prints the ext list */
void print_ext(ext_ptr h) {
    ext_ptr orig = h;
    do {
        printf("\nname: %s, reference: %d - >", h->name, h->address);
        h = h->next;
    } while (h->address != orig->address);
    printf("*\n");
}
