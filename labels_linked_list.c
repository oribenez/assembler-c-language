#include "labels_linked_list.h"
#include <stdio.h>

/* This function offsets the addresses of a certain group of labels (data/instruction labels)
 * by a given delta (num).
 */
void proceed_addr(label_ptr label, int num, bool is_data) {
    while (label) {
        /* We don't offset external labels (their address is 0). */
        /* is_data and activeRow must have different values in order to meet the same criteria
         * and the XOR operator gives us that */
        if (!(label->external) && (is_data ^ (label->activeRow))) {
            label->address += num;
        }
        label = label->next;
    }
}

/* This function searches a label in the list and changes his entry field to TRUE and returns TRUE
else if the label doesn't exist return FALSE. */
int set_label_to_entry(label_ptr h, char *name) {
    label_ptr label = get_label(h, name);
    if (label != NULL) {
        if (label->external) {
            set_error("ENTRY_CANT_BE_EXTERN");
            return FALSE;
        }
        label->entry = TRUE;
        entry_exists = TRUE; /* Global variable that holds that there was at least one entry in the program */
        return TRUE;
    } else
        set_error("LABEL_DOES_NOT_EXIST");

    return FALSE;
}

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

/* This function returns the address of a given label, if the label doesn't exist return FALSE (0).*/
unsigned int get_label_addr(label_ptr h, char *name) {
    label_ptr label = get_label(h, name);
    if (label != NULL)
        return label->address;
    return FALSE;
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
        temp->activeRow = va_arg(p, bool);
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

/* This function check if a label is in the array and an external label is so return 1 else return 0 */
bool is_label_external(label_ptr hptr, char *name) {
    
    label_ptr label = get_label(hptr, name);
    if (label != NULL)
        return label->external;
    return FALSE;
}

/* This function prints the list */
void print_labels(label_ptr h) {
    while (h) {
        printf("\nname: %s, address: %d, external: %d", h->name, h->address, h->external);
        if (h->external == 0)
            printf(", is in action statement: %d -> ", h->activeRow);
        else
            printf(" -> ");
        h = h->next;
    }
    printf("*");
}
