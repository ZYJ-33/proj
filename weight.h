#include <stdlib.h>
#include <sys/types.h>
#ifndef WEIGHT
#define WEIGHT
struct weight_node
{
    u_int64_t weight;
    struct weight_node* next;
};

struct weight_list
{
    struct weight_node* header;
    struct weight_node* tail;
};


struct weight_list* new_list();
void add_weight(struct weight_list* l, u_int64_t weight);
void init_weight(char* ws, u_int32_t len);
#endif
