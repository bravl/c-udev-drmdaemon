/** @file list.c
 *  @brief Functions for the linked list.
 *
 *  This file contains the code to control the linked list directly
 *	Source used: Mastering algorithms in C
 *
 *  @author Bram Vlerick (vlerickb@gmail.com)
 *  @bug
 *  * None at the moment
 * TODO: Optimize list operations
 */

#include "list.h"

struct dlist *list_init(void (*destroy)(void *data)) {
    /* Initialise struct*/
    struct dlist *list = malloc(sizeof(struct dlist));
    if (!list) {
        return NULL;
    }
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    list->destroy = destroy;
    return list;
}

void list_destroy(struct dlist *list) {
    void *data;
    if (!list) {
        return;
    }
    /* loop through list*/
    while (LIST_SIZE(list) > 0) {

        /* remove element from list*/
        if (list_remove_item(list, LIST_TAIL(list), &data) == 0 &&
            list->destroy != NULL) {
            /* cleanup data*/
            list->destroy(data);
        }
    }
    /* clear list*/
    memset(list, 0, sizeof(struct dlist));
    free(list);
    list = NULL;
    return;
}

int list_insert_next(struct dlist *list, struct dlist_element *element, const void *data) {
    if (!list) {
        return -1;
    }
    struct dlist_element *new_element;
    /* check params*/
    if (element == NULL && LIST_SIZE(list) != 0)
        return -1;

    /* allocate memory for element*/
    if ((new_element = malloc(sizeof(struct dlist_element))) == NULL)
        return -1;

    /* link data pointers*/
    new_element->data = (void *)data;

    /* if the list is empty*/
    if (LIST_SIZE(list) == 0) {
        list->head = new_element;
        list->head->prev = NULL;
        list->head->next = NULL;
        list->tail = new_element;
    } else {
        /* Not empty*/
        /* insert after selected element*/
        new_element->next = element->next;
        new_element->prev = element;
        if (element->next == NULL)
            list->tail = new_element;
        else
            element->next->prev = new_element;
        element->next = new_element;
    }
    /* increment list size*/
    list->size++;
    return 0;
}

int list_insert_prev(struct dlist *list, struct dlist_element *element, const void *data) {
    if (!list) {
        return -1;
    }
    struct dlist_element *new_element;

    /* parameter check*/
    if (element == NULL && LIST_SIZE(list) != 0)
        return -1;

    /* allocate memory*/
    if ((new_element = malloc(sizeof(struct dlist_element))) == NULL)
        return -1;

    /* link data pointers*/
    new_element->data = (void *)data;

    /* list is empty*/
    if (LIST_SIZE(list) == 0) {
        list->head = new_element;
        list->head->prev = NULL;
        list->head->next = NULL;
        list->tail = new_element;
    } else {
        /* list not empty*/
        /* insert before selected element*/
        new_element->next = element;
        new_element->prev = element->prev;
        if (element->prev == NULL)
            list->head = new_element;
        else
            element->prev->next = new_element;
        element->prev = new_element;
    }

    /* increment list size*/
    list->size++;
    return 0;
}

int list_remove_item(struct dlist *list, struct dlist_element *element, void **data) {
    if (!list) {
        return -1;
    }
    /* parameter check*/
    if (element == NULL || LIST_SIZE(list) == 0)
        return -1;

    /* retrieve data pointer*/
    *data = element->data;
    if (element == list->head) {
        /* if it's the first element*/
        list->head = element->next;
        if (list->head == NULL)
            list->tail = NULL;
        else
            element->next->prev = NULL;
    } else {
        /* not the first element*/
        element->prev->next = element->next;
        if (element->next == NULL)
            list->tail = element->prev;
        else
            element->next->prev = element->prev;
    }

    /* memory cleanup*/
    free(element);

    /* decrement list*/
    list->size--;
    return 0;
}
