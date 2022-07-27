/**
 * @file external_linked_list.c
 * @brief this file includes all the functions which are managing the linked list of all external addresings.
 */

#include "external_linked_list.h"
#include <stdio.h>

/**
 * @brief function which adds a given info to a new node in the given external_list
 * 
 * @param hptr the list to insert to a new node
 * @param name the name of the node to insert
 * @param reference the addr to insert
 * @return ext_ptr pointer to the new node
 */
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

/**
 * @brief free an allocated memory of a given list
 * 
 * @param hptr the head of the list to free
 */
void ext_free_list(ext_ptr *hptr) {
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
