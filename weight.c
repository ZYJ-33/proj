#include <stdlib.h>
#include <sys/types.h>
#include "./weight.h"
char* weights;
u_int32_t weight_cur_index = 0;
u_int32_t weight_len = 0;

void init_weight(char* ws, u_int32_t len)
{
    weights = ws;
    weight_len = len;
}

struct weight_node* new_node(u_int64_t weight)
{
    struct weight_node* n = (struct weight_node*) malloc(sizeof(*n));
    n->weight = weight;
    n->next = 0;
    return n;
}

struct weight_list* new_list()
{
    struct weight_list* l = (struct weight_list*) malloc(sizeof(*l));
    l->header = 0;
    l->tail = 0;
    return l;
}

void add_weight(struct weight_list* l, u_int64_t weight)
{
    struct weight_node* n = new_node(weight);
    if (l->header == 0)
    {
        l->header = n;
        l->tail = n;
    }
    else
    {
       l->tail->next = n;
       l->tail = n; 
    }
}
