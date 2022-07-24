#include "inner_node.h"
#include "node.h"
#include "ast.h"
#include "meta_type.h"
#include "./transi.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void travel_val(struct val_node* val)
{
    if (val == 0)
            return;
    switch (val->type) 
    {
        case STR:
        {
            printf("%s\n", val->str);
            break;
        }
        case NUM:
        {
            printf("%d\n", val->num);
            break;
        }
        case BOOL:
        {
            if(val->boolean == 1)
                    printf("true");
            else
                    printf("false");
            break;
        }
        case ARR:
        {
            travel_arr(val->arr);
            break;
        }
        case OBJ:
        {
            travel_obj(val->obj);
            break;
        }
        default:
        {
            printf("value unknow\n");
            exit(1);
        }
    }
}

void travel_arr(struct array_node* cur)
{
    printf("[\n");
    while(cur->val)
    {
        travel_val(cur->val);
        printf(" , ");
        cur = cur->next;
    }
    printf("]\n");
}

void travel_obj(struct obj_node* cur)
{
    printf("{\n");
    while(cur->kv)
    {
        printf("%s: ", cur->kv->key);
        travel_val(cur->kv->val);
        printf(" , ");
        cur = cur->next;
    }
    printf("}\n");
}

struct val_node* transit_val(struct basic_node* node)
{
    struct val_node* val = (struct val_node*)malloc(sizeof(*val));
    memset((void*)val, 0, sizeof(*val)); 
    struct data_node* dn = (struct data_node*)node;
    struct one_child_node* on = (struct one_child_node*)node;
    switch (node->type)
    {
        case STRING_NODE:
                {
                    val->type = STR;
                    val->str = dn->data;
                    dn->data = 0;
                    break;
                }
        case NUM_NODE:
                {
                    val->type = NUM;
                    val->num = (int64_t)dn->data;
                    break;
                }
        case BOOL_NODE:
                {
                    val->type = BOOL;
                    val->boolean = (int64_t)dn->data;
                    break;
                }
        case OBJ_VAL_NODE:
                {
                    val->type = OBJ;
                    struct obj_node* new_obj = transit_obj((struct two_child_node*)on->fst);
                    val->obj = new_obj;
                    break;
                }
        case ARRAY_VAL_NODE:
                {
                    val->type = ARR;
                    struct array_node* new_arr = transit_arr((struct two_child_node*)on->fst);
                    val->arr = new_arr;
                    break;
                }
        default:
                {
                    printf("in trainsit_val\n ");
                    exit(1);
                } 

    }
    return val;
}

struct array_node* transit_arr(struct two_child_node* node)
{
   struct array_node* arr = (struct array_node*)malloc(sizeof(*arr));
   memset((void*)arr, 0, sizeof(*arr));
   if(node->fst == 0)
   {
           arr->next = 0;
           arr->val = 0;
           return arr;
   }
   struct val_node* val = (struct val_node*)transit_val(node->fst);
   arr_push(arr, val);
   struct two_child_node* next = (struct two_child_node*)node->snd;
   while(next)
   {
        val = (struct val_node*)transit_val(next->fst);
        arr_push(arr, val);
        next = (struct two_child_node*)next->snd;
   }
   return arr;
}

struct obj_node* transit_obj(struct two_child_node* node)
{
    struct obj_node* obj = (struct obj_node*)malloc(sizeof(*obj));
    memset((void*)obj, 0, sizeof(*obj));
    if(node->fst == 0)
    {
            obj->kv = 0;
            obj->next = 0;
            return obj;
    }
    transit_pair(obj, (struct key_child_node*)node->fst);
    struct basic_node* next = node->snd;
    while(next)
    {
        struct two_child_node* now = (struct two_child_node*)next;
        transit_pair(obj, (struct key_child_node*)now->fst);
        next = now->snd;
    }
    return obj;
} 

void transit_pair(struct obj_node* obj, struct key_child_node* node)
{
    if(node->meta_type != META_KEY_CHILD)
    {
        printf("in transit_pair\n");
        exit(1);
    }
    struct val_node* val = transit_val((struct basic_node*)node->child);
    obj_set(obj, node->key, val);
}

