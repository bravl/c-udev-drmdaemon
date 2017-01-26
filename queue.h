/**
 * @file queue.h
 * @Brief  Queue implementation based on dlist
 * @author Bram Vlerick
 * @version 1.0
 * @date 2017-01-26
 */

#include "list.h"

#define queue_element dlist_element
#define queue dlist_t

#define queue_init list_init
#define queue_destroy list_destroy
#define queue_push(x, y) dlist_insert_next(x, x->tail, y)
#define queue_pop(x, y) dlist_remove_item(x, x->head, y)
