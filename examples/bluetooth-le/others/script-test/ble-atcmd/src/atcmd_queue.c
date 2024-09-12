#include "atcmd_queue.h"

// PRIVATE VARIABLE DECLARE
static int front_index = 0;
static int rear_index = 0;
static atcmd_node_t node_arr[NODE_ARR_MAX_SIZE];

// PRIVATE FUNCTION DECLARE
static atcmd_node_t *new_node_get(void);
static void node_delete(void);
static void  atcmd_queue_push  (atcmd_queue_t *q, ITEM *new_val);
static void  atcmd_queue_pop   (atcmd_queue_t *q);
static ITEM *atcmd_queue_front (atcmd_queue_t *q);
static int   atcmd_queue_size  (atcmd_queue_t *q);
static bool  atcmd_queue_empty (atcmd_queue_t *q);
static bool  atcmd_queue_full  (atcmd_queue_t *q);
static void  atcmd_queue_print (atcmd_queue_t *q);

// PUBLIC FUNCTION IMPLEMENT
void atcmd_queue_init(atcmd_queue_t *q)
{
    q->front_node = NULL;
    q->rear_node = NULL;
    q->length = 0;

    q->push = atcmd_queue_push;
    q->pop = atcmd_queue_pop;
    q->front = atcmd_queue_front;
    q->size = atcmd_queue_size;
    q->empty = atcmd_queue_empty;
    q->full = atcmd_queue_full;
    q->Print = atcmd_queue_print;
}
void atcmd_queue_clear(atcmd_queue_t *q)
{
    while (q->length > 0)
    {
        q->pop(q);
    }
}

// PRIVATE FUNCTION IMPLEMENT
static atcmd_node_t *new_node_get()
{
    rear_index++;
    if (rear_index == NODE_ARR_MAX_SIZE)
    {
        rear_index = 0;
    }

    return &node_arr[rear_index];
}
static void node_delete()
{
    front_index++;
    if (front_index == NODE_ARR_MAX_SIZE)
    {
        front_index = 0;
    }
}
static void atcmd_queue_push(atcmd_queue_t *q, ITEM *new_val)
{
    atcmd_node_t *node;

    if (q->length == NODE_ARR_MAX_SIZE)
    {
        PRINT_ERROR_MSG;
        return;
    }

    node = new_node_get();
    node->val = *new_val;
    node->next = NULL;

    if (q->length == 0)
    {
        q->front_node = node;
        q->rear_node = node;
    }
    else
    {
        q->rear_node->next = node;
        q->rear_node = q->rear_node->next;
    }
    q->length++;
    return;
}
static void atcmd_queue_pop(atcmd_queue_t *q)
{
    if (q->length == 0)
    {
        return;
    }
    q->front_node = q->front_node->next;
    if (q->length == 1)
    {
        q->rear_node = NULL;
    }

    node_delete();
    q->length--;
    return;
}
static ITEM *atcmd_queue_front(atcmd_queue_t *q)
{
    if (q->empty(q))
    {
        PRINT_ERROR_MSG;
    }
    return &(q->front_node->val);
}
static int atcmd_queue_size(atcmd_queue_t *q)
{
    return q->length;
}
static bool atcmd_queue_empty(atcmd_queue_t *q)
{
    return q->length == 0;
}
static bool atcmd_queue_full(atcmd_queue_t *q)
{
    return q->length == NODE_ARR_MAX_SIZE;
}
static void atcmd_queue_print(atcmd_queue_t *q)
{
    atcmd_node_t *node = q->front_node;

    printf("size:%d \n", q->length);
    while (node != NULL)
    {
        PRINT_ITEM(node);
        printf("->\n");
        node = node->next;
    }

    printf("NULL\n");
    return;
}

