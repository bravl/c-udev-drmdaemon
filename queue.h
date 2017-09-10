/**
 * @file queue.h
 * @Brief  Queue implementation based on dlist
 * @author Bram Vlerick
 * @version 1.0
 * @date 2017-01-26
 */

/* Just some defines.. See list.h/.c for more info */

#include "list.h"

#define queue_element dlist_element
#define queue dlist

#define queue_init list_init
#define queue_destroy list_destroy
#define queue_push(x, y) list_insert_next(x, x->tail, y)
#define queue_pop(x, y) list_remove_item(x, x->head, y)

#define QUEUE_SIZE(queue) LIST_SIZE(queue)
