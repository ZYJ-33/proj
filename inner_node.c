#include "inner_node.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

struct val_node* init(int type, void* data)
{
    struct val_node* n = (struct val_node*) malloc(sizeof(*n));
    n->type = type;
    switch (type)
    {
        case STR:
        {
            n->str = (char*)data;
            break;
        }
        case NUM:
        {
            n->num = (int64_t)data;
            break;
        }
        case BOOL:
        {
            n->boolean = (int64_t)data;
            break;
        }
        case ARR:
        {
            n->arr = (struct array_node*)data;
            break; 
        }
        case OBJ:
        {
            n->obj = (struct obj_node*)data;
            break;
        }
        default:
        {
            printf("unknow type\n");
            exit(1);
        }
    }
    return n;
}

struct val_node* arr_get(struct array_node* arr, int i)
{
    int j=0;
    while(arr != 0 && j < i)
    {
        arr = arr->next;
        j++;
    }
    return arr->val;
}

int arr_push(struct array_node *arr, struct val_node *val)
{
    while(arr->val != 0)
    {
        arr = arr->next;
    }
    struct array_node* new_one = (struct array_node*)malloc(sizeof(*new_one));
     new_one->next = 0;
     new_one->val = 0;
     arr->val = val;
     arr->next = new_one;
     return 1;
}

struct val_node* obj_get(struct obj_node* obj, char* key)
{
    struct obj_node* cur = obj;
    while(cur)
    {

        if(cur->kv != 0 && strcmp(cur->kv->key, key) == 0)
           return obj->kv->val;
        cur = cur->next;
    } 
    return 0;
}

int obj_set(struct obj_node *obj, char *key, struct val_node *val)
{
    struct obj_node* cur = obj;
    struct val_node* res = obj_get(obj, key);
    if (res != 0)
    {
        printf("alreay exist %s\n", key);
        exit(1);
    }

    while(cur->kv != 0)
        cur = cur->next;

    struct obj_node* new = (struct obj_node*) malloc(sizeof(*new));
    new->next = 0;
    new->kv = 0;
    cur->kv = (struct key_val_node*) malloc(sizeof(*(new->kv)));
    cur->kv->key = key;
    cur->kv->val = val;
    cur->next = new;
    return 1;
}
