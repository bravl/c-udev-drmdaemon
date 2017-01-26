/** @file list.h
 *  @brief Functions prototypes for the linked list.
 *
 *  This file contains the prototypes and macros to control the linked
 *  list directly
 *	Source used: Mastering algorithms in C
 *
 *  @author Bram Vlerick (vlerickb@gmail.com)
 *  @bug
 *  * None at the moment
 *
 *	This code works, no need for changes
 */

#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

/** @brief defintion of the list element structure
 *
 *  This structure contains the data for a list element
 */
struct dlist_element {
    /**< void pointer to element data */
    void *data;
    /**< pointer to previous element in the list */
    struct dlist_element *prev;
    /**< pointer to next element in the list */
    struct dlist_element *next;
};

/** @brief definition of the list structure
 *
 *  This structure contains the data for a list.
 */
struct dlist{
    /**< total amount elements in the list*/
    int size;

    /**< function pointer to the match functions*/
    int (*match)(const void *key1, const void *key2);

    /**< function pointer to the destroy function */
    void (*destroy)(void *data);

    /**< the first element of the list*/
    struct dlist_element *head;

    /**< the last element of the list*/
    struct dlist_element *tail;
    pthread_mutex_t list_mutex;
};

/**@brief Macro to get the tail of a list
 */
#define LIST_TAIL(list) ((list)->tail)

/**@brief Macro to get the head of a list
 */
#define LIST_HEAD(list) ((list)->head)

/**@brief Macro to get the list size
 */
#define LIST_SIZE(list) ((list)->size)

/**@brief Macro to check if it's the head of the list
 */
#define IS_LIST_HEAD(element) ((element)->prev == NULL ? 1 : 0)

/**@brief Macro to check if it's the tail of the list
 */
#define IS_LIST_TAIL(element) ((element)->next == NULL ? 1 : 0)

/** @brief This function will initialise the list
 *
 *  This function if responsible for initializing the list correctly
 *
 *  @param list A pointer to the list that has to be initialized
 *  @param destroy function pointer to the function that is used to remove the
 * data
 */
struct dlist_t *list_init(void (*destroy)(void *data));

/** @brief This function will destroy the list
 *
 *  This function is responsible for removing all elements and their data. When
 * this
 *  is done it will destroy the list itself
 *
 *  @param list the list that has to be removed
 */
void list_destroy(dlist_t *list);

/** @brief Insert an element after another element
 *
 *  This function will insert a new element with its data into the list behind a
 * selected
 *  element
 *
 *  @param list the list where the element will be inserted into
 *  @param element pointer to the element whereafter we will insert the new
 * element
 *  @param data the data that will be stored by the new element
 *  @return function return 0 if successful
 */
int list_insert_next(struct dlist_t *list, struct dlist_element *element, const void *data);

/** @brief Insert an element after before element
 *
 *  This function will insert a new element with its data into the list before a
 * selected
 *  element
 *
 *  @param list the list where the element will be inserted into
 *  @param element pointer to the element where we will insert the element
 * infront
 *  @param data the data that will be stored by the new element
 *  @return function will return 0 if successful
 */
int list_insert_prev(struct dlist_t *list, struct dlist_element *element, const void *data);

/** @brief Remove an element from the list
 *
 *  This function will remove a selected element from the list. It will return
 * the data
 *  from the removed element in the data pointer.
 *  @param list the list where the element will be removed from
 *  @param element the element that has to be removed
 *  @param data the pointer to the data pointer of the element that is removed
 *  @return the function returns 0 if successful
 *  @warning programmer is resposible for removing data after element is removed
 */
int list_remove_item(struct dlist_t *list, struct dlist_element *element, void **data);

#endif
