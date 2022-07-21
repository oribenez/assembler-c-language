#include "labelsLinkedList.h"
#include <stdio.h>

/**
 * @brief function which get a pointer to labels list and free memory allocation.
 *
 * @param hptr the HEAD of the linked list to free
 */
void free_labels(label_ptr *hptr) {
    /* Free the label list by going over each label and free it */
    label_ptr temp;
    while (*hptr) {
        temp = *hptr;
        *hptr = (*hptr)->next;
        free(temp);
    }
}

/**
 * @brief Deletes label from linked list of the given name
 *
 * @param hptr pointer to the head of the linked list
 * @param name  the name of the item to delete
 * @return true if label deleted successfuly
 */
status delete_label(label_ptr *hptr, char *name) {
    label_ptr temp = *hptr;
    label_ptr prev_temp;
    while (temp) {
        if (strcmp(temp->name, name) == 0) {
            if (strcmp(temp->name, (*hptr)->name) == 0) {
                *hptr = (*hptr)->next;
                free(temp);
            } else {
                prev_temp->next = temp->next;
                free(temp);
            }
            return SUCCESS;
        }
        prev_temp = temp;
        temp = temp->next;
    }
    return FAILED;
}

/**
 * @brief Get the label node from linked list
 *
 * @param hptr pointer to HEAD of linked list
 * @param name name of the label to find
 * @return label_ptr pointer to node withe the given label
 */
label_ptr get_label(label_ptr hptr, char *name) {
    while (hptr) {
        if (!strcmp(hptr->name, name)) /* we found a label with the name given */
            return hptr;
        hptr = hptr->next;
    }
    return NULL;
}

/* This function checks if a given name is a name of a label in the list */
bool is_existing_label(label_ptr hptr, char *name) {
    return get_label(hptr, name) != NULL;
}

/**
 * @brief function to add a label to symbols list
 *
 * @param hptr a node of the linked list
 * @param name
 * @param address
 * @param external
 * @param ...
 * @return label_ptr
 */
label_ptr insert_label(label_ptr *hptr, char *name, unsigned int address, bool external, ...) {
    va_list p;

    label_ptr t = *hptr;
    label_ptr temp; /* Auxiliary variable to store the info of the label and add to the list */

    if (is_existing_label(*hptr, name)) {
        set_error("LABEL_ALREADY_EXISTS");
        return NULL;
    }
    temp = (label_ptr)malloc_with_check(sizeof(Labels));

    /* Storing the info of the label in temp */
    strcpy(temp->name, name);
    temp->entry = FALSE;
    temp->address = address;
    temp->external = external;

    if (!external) /* An external label can't be in an action statement */
    {
        va_start(p, external);
        temp->inActionStatement = va_arg(p, bool);
    } else {
        extern_exists = TRUE;
    }

    /* If the list is empty then we set the head of the list to be temp */
    if (!(*hptr)) {
        *hptr = temp;
        temp->next = NULL;
        return temp;
    }

    /* Setting a pointer to go over the list until he points on the last label and then stting temp to be the new last label */
    while (t->next != NULL)
        t = t->next;
    temp->next = NULL;
    t->next = temp;

    va_end(p);
    return temp;
}
