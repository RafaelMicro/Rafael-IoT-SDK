#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdio.h>
#include "stdbool.h"
#include "atcmd_parser.h"

#define NODE_ARR_MAX_SIZE 4

#define PRINT_ERROR_MSG printf("queue error\n")

#define ITEM atcmd_string_t
#define PRINT_ITEM(node) atcmd_string_print(&(node->val))

typedef struct atcmd_node_s
{
    ITEM val;
    struct atcmd_node_s *next;
} atcmd_node_t;

typedef struct atcmd_queue_s
{
    atcmd_node_t *front_node;
    atcmd_node_t *rear_node;
    int length;

    void  (*push)  (struct atcmd_queue_s *q, ITEM *new_val);
    void  (*pop)   (struct atcmd_queue_s *q);
    ITEM *(*front) (struct atcmd_queue_s *q);
    int   (*size)  (struct atcmd_queue_s *q);
    bool  (*empty) (struct atcmd_queue_s *q);
    bool  (*full)  (struct atcmd_queue_s *q);
    void  (*Print) (struct atcmd_queue_s *q);

} atcmd_queue_t;

void atcmd_queue_init(atcmd_queue_t *q);
void atcmd_queue_clear(atcmd_queue_t *q);

#endif //_QUEUE_H_
