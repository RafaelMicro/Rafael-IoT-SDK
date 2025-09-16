#include "slist.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************/ /**
                                                                               * Initializes a singly-linked list.
                                                                               ******************************************************************************/
void slist_init(slist_node_t** head) { *head = 0; }

/***************************************************************************/ /**
                                                                               * Add given item at beginning of list.
                                                                               ******************************************************************************/
void slist_push(slist_node_t** head, slist_node_t* item) {
    // EFM_ASSERT((item != NULL) && (head != NULL));
    item->node = *head;
    *head = item;
}

/***************************************************************************/ /**
                                                                               * Add item at end of list.
                                                                               ******************************************************************************/
void slist_push_back(slist_node_t** head, slist_node_t* item) {
    // EFM_ASSERT((item != NULL) && (head != NULL));
    slist_node_t** node_ptr = head;
    while (*node_ptr != NULL)
        node_ptr = &((*node_ptr)->node);

    item->node = NULL;
    *node_ptr = item;
}

/***************************************************************************/ /**
                                                                               * Removes and returns first element of list.
                                                                               ******************************************************************************/
slist_node_t* slist_pop(slist_node_t** head) {
    // EFM_ASSERT(head != NULL);
    slist_node_t* item = *head;
    if (item == NULL)
        return (NULL);

    *head = item->node;
    item->node = NULL;
    return (item);
}

/***************************************************************************/ /**
                                                                               * Insert item after given item.
                                                                               ******************************************************************************/
void slist_insert(slist_node_t* item, slist_node_t* pos) {
    // EFM_ASSERT((item != NULL) && (pos != NULL));

    item->node = pos->node;
    pos->node = item;
}

/***************************************************************************/ /**
                                                                               * Remove item from list.
                                                                               ******************************************************************************/
void slist_remove(slist_node_t** head, slist_node_t* item) {
    slist_node_t** node_ptr;

    // EFM_ASSERT((item != NULL) && (head != NULL));

    for (node_ptr = head; *node_ptr != NULL; node_ptr = &((*node_ptr)->node)) {
        if (*node_ptr == item) {
            *node_ptr = item->node;
            return;
        }
    }

    // EFM_ASSERT(node_ptr != NULL);
}

/***************************************************************************/ /**
                                                                               * Sorts list items.
                                                                               ******************************************************************************/
void slist_sort(slist_node_t** head,
                bool (*cmp_fnct)(slist_node_t* item_l, slist_node_t* item_r)) {
    // EFM_ASSERT((head != NULL) && (cmp_fnct != NULL));
    bool swapped;
    slist_node_t** pp_item_l;

    do {
        swapped = false;

        pp_item_l = head;
        // Loop until end of list is found.
        while ((*pp_item_l != NULL) && ((*pp_item_l)->node != NULL)) {
            slist_node_t* p_item_r = (*pp_item_l)->node;
            // Call provided compare fnct.
            if (cmp_fnct(*pp_item_l, p_item_r)) {
                pp_item_l = &((*pp_item_l)->node);
                continue;
            }
            // If order is not correct, swap items.
            slist_node_t* p_tmp = p_item_r->node;
            p_item_r->node = *pp_item_l;
            (*pp_item_l)->node = p_tmp;
            *pp_item_l = p_item_r;

            pp_item_l = &(p_item_r->node);
            // Indicate a swap has been done.
            swapped = true;
        }
        // Re-loop until no items have been swapped.
    } while (swapped == true);
}
