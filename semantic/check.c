#include "../inner_node.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define TRUE  1
#define FALSE 0
#define BOOL_TYPE int

void panic(char* msg)
{
    printf("%s\n", msg);
    exit(1);
}

void check_conv3(struct obj_node* obj)
{
    struct obj_node* cur = obj;
    BOOL_TYPE type = FALSE;
    BOOL_TYPE pad_left = FALSE;
    BOOL_TYPE pad_right = FALSE;
    BOOL_TYPE pad_top  = FALSE;
    BOOL_TYPE pad_bot  = FALSE;
    BOOL_TYPE channum = FALSE;
    BOOL_TYPE weight_file = FALSE;
    BOOL_TYPE bias = FALSE;

    while(cur->next)
    {
        struct key_val_node* kv = cur->kv;
        if(strcmp(kv->key, "type") == 0)
        {
            type = TRUE;
        }
        else if(strcmp(kv->key, "padding_left") == 0)
        {
            pad_left = TRUE;
        }
        else if(strcmp(kv->key, "padding_right") == 0)
        {
            pad_right = TRUE;
        }
        else if(strcmp(kv->key, "padding_top") == 0)
        {
            pad_top = TRUE;
        }
        else if(strcmp(kv->key, "padding_bot") == 0)
        {
            pad_bot = TRUE;
        }
        else if(strcmp(kv->key, "chan_num") == 0)
        {
            channum = TRUE;
        }
        else if(strcmp(kv->key, "weight_file") == 0)
        {
            weight_file = TRUE;
        }
        else if(strcmp(kv->key, "bias") == 0)
        {
            bias = TRUE;
        }
        else
        {
            printf("unknow keyword %s found in conv3", kv->key);
            panic("");
        }

        cur = cur->next;
    }
    if(!(channum && weight_file))
    {
        if(!channum)
          panic("conv3 have to specify chan_num\n");
        if(!weight_file)
          panic("conv3 have to specify weight_file\n");
    }
}


void check_conv1(struct obj_node* obj)
{
    struct obj_node* cur = obj;
    BOOL_TYPE type = FALSE;
    BOOL_TYPE channum = FALSE;
    BOOL_TYPE weight_file = FALSE;
    BOOL_TYPE bias = FALSE;

    while(cur->next)
    {
        struct key_val_node* kv = cur->kv;
        if(strcmp(kv->key, "type") == 0)
        {
            type = TRUE;
        }
        else if(strcmp(kv->key, "chan_num") == 0)
        {
            channum = TRUE;
        }
        else if(strcmp(kv->key, "weight_file") == 0)
        {
            weight_file = TRUE;
        }
        else if(strcmp(kv->key, "bias") == 0)
        {
            bias = TRUE;
        }
        else
        {
            printf("unknow keyword %s found in conv1", kv->key);
            panic("");
        }

        cur = cur->next;
    }
    if(!(channum && weight_file))
    {
        if(!channum)
          panic("conv1 have to specify chan_num\n");
        if(!weight_file)
          panic("conv1 have to specify weight_file\n");
    }

}

void check_activate(struct obj_node* obj)
{
    struct obj_node* cur = obj;
    BOOL_TYPE pos_slope = FALSE;
    BOOL_TYPE neg_slope = FALSE;
    BOOL_TYPE pos_thd = FALSE;
    BOOL_TYPE neg_thd = FALSE;
    while(cur->next)
    {
       struct key_val_node* kv = cur->kv;
       if(strcmp(kv->key, "pos_slope") == 0)
       {
            pos_slope = TRUE;
       }
       else if(strcmp(kv->key, "neg_slope") == 0)
       {
            neg_slope = TRUE;
       }
       else if(strcmp(kv->key, "pos_thd") == 0)
       {
            pos_thd = TRUE;
       }
       else if(strcmp(kv->key, "neg_thd") == 0)
       {
            neg_thd = TRUE;
       }
       cur = cur->next;
    }
    if(!(pos_slope && neg_slope))
    {
        if(pos_slope)
           panic("activate should have pos_slope");
        if(neg_slope)
           panic("activate should have neg_slope");
    }
}

void check_quantize(struct obj_node* obj)
{
    struct obj_node* cur = obj;
    BOOL_TYPE pos_slope = FALSE;
    BOOL_TYPE neg_slope = FALSE;
    while(cur->next)
    {
       struct key_val_node* kv = cur->kv;
       if(strcmp(kv->key, "pos_slope") == 0)
       {
            pos_slope = TRUE;
       }
       else if(strcmp(kv->key, "neg_slope") == 0)
       {
            neg_slope = TRUE;
       }
       cur = cur->next;
    }
    if(!(pos_slope && neg_slope))
    {
        if(pos_slope)
           panic("quantize should have pos_slope");
        if(neg_slope)
           panic("quantize should have neg_slope");
    }
}


void check_operators(struct obj_node* obj)
{
    if(obj == 0)
        return;
    struct obj_node* cur = obj;
    BOOL_TYPE at_least_one = FALSE;
    while(cur->next)
    { 
        struct key_val_node* kv = obj->kv;
        if(strcmp(kv->key, "conv3") == 0)
        {
            at_least_one = TRUE;
            check_conv3(kv->val->obj);    
        }
        else if(strcmp(kv->key, "conv1") == 0)
        {
            at_least_one = TRUE;
            check_conv1(kv->val->obj);
        }
        else if(strcmp(kv->key, "quantize") == 0)
        {
            at_least_one = TRUE;
            check_quantize(kv->val->obj);
        }
        else if(strcmp(kv->key, "maxpool") == 0)
        {
            if(kv->val != 0)
                   panic("maxpool don't have parameter");
            at_least_one = TRUE;
        }
        else if(strcmp(kv->key, "activate") == 0)
        {
            at_least_one = TRUE;
            check_activate(kv->val->obj);
        }
        else
        {
            printf("did not impl operator %s\n", kv->key);
            panic("");
        }
        cur = cur->next;
    } 

    if(!at_least_one)
            panic("operators should have one operator at least");

}

void check_inputaddr(struct obj_node* obj)
{
    struct obj_node* cur = obj;
    BOOL_TYPE th = FALSE;
    BOOL_TYPE tw = FALSE;
    BOOL_TYPE addr = FALSE;
    BOOL_TYPE bpl = FALSE;
    BOOL_TYPE bpc = FALSE;
    while(cur->next)
    {
        struct key_val_node* kv = cur->kv;
        if(strcmp(kv->key, "addr") == 0)
        {
            addr = TRUE;
        }
        else if(strcmp(kv->key, "tile_height") == 0)
        {
            th = TRUE;
        }
        else if(strcmp(kv->key, "tile_width") == 0)
        {
            tw = TRUE;
        }
        else if(strcmp(kv->key, "byte_perline") == 0)
        {
            bpl = TRUE;
        }
        else if(strcmp(kv->key, "byte_perchan") == 0)
        {
            bpc = TRUE;
        }
        else 
        {
            printf("unknow keyword %s in input_addr", kv->key);
            panic("");
        }
        cur = cur->next;
    }
    if(!(bpc && bpl && addr && th && tw))
            panic("missing keyword in input_addr");
}

void check_writeback(struct obj_node* obj)
{
    struct obj_node* cur = obj;
    BOOL_TYPE addr = FALSE;
    BOOL_TYPE bpl = FALSE;
    BOOL_TYPE bpc = FALSE;
    while(cur->next)
    {
        struct key_val_node* kv = cur->kv;
        if(strcmp(kv->key, "addr") == 0)
        {
            addr = TRUE;
        }
        else if(strcmp(kv->key, "byte_perline") == 0)
        {
            bpl = TRUE;
        }
        else if(strcmp(kv->key, "byte_perchan") == 0)
        {
            bpc = TRUE;
        }
        else 
        {
            printf("unknow keyword %s in write_back", kv->key);
            panic("");
        }
        cur = cur->next;
    }
    if(!(bpc && bpl && addr))
            panic("missing keyword in write_back");
       
}

void check_otheraddr(struct obj_node* obj)
{
    struct obj_node* cur = obj;
    BOOL_TYPE th = FALSE;
    BOOL_TYPE tw = FALSE;
    BOOL_TYPE addr = FALSE;
    BOOL_TYPE bpl = FALSE;
    BOOL_TYPE bpc = FALSE;
    while(cur->next)
    {
        struct key_val_node* kv = cur->kv;
        if(strcmp(kv->key, "addr") == 0)
        {
            addr = TRUE;
        }
        else if(strcmp(kv->key, "tile_height") == 0)
        {
            th = TRUE;
        }
        else if(strcmp(kv->key, "tile_width") == 0)
        {
            tw = TRUE;
        }
        else if(strcmp(kv->key, "byte_perline") == 0)
        {
            bpl = TRUE;
        }
        else if(strcmp(kv->key, "byte_perchan") == 0)
        {
            bpc = TRUE;
        }
        else 
        {
            printf("unknow keyword %s in other_addr", kv->key);
            panic("");
        }
        cur = cur->next;
    }
    if(!(bpc && bpl && addr && th && tw))
            panic("missing keyword in other_addr");
}


void check_instr(struct obj_node* instr)
{
    struct obj_node* cur = instr;
    BOOL_TYPE operators = FALSE;
    BOOL_TYPE input_addr = FALSE;
    BOOL_TYPE write_back = FALSE;
    BOOL_TYPE other_addr = FALSE;
    while(cur->next)
    {
        struct key_val_node* kv = cur->kv;
        if(kv->val->type != OBJ)
        {
            printf("%s should be a object\n", kv->key);
            panic("");
        }
        if(strcmp(kv->key, "operators") == 0)
        {
            operators = TRUE;
            check_operators(kv->val->obj);          
        }
        else if(strcmp(kv->key, "input_addr") == 0)
        {
            input_addr = TRUE;
            check_inputaddr(kv->val->obj);
        }
        else if(strcmp(kv->key, "other_addr") == 0)
        {
             other_addr = TRUE;
             check_otheraddr(kv->val->obj);
        }
        else if(strcmp(kv->key, "write_back") == 0)
        {
              write_back = TRUE;
              check_writeback(kv->val->obj);
        }
        else
        {
            printf("unknow keyword %s in instr\n", kv->key);
            panic("");
        }
        cur = cur->next;
    }
    if(!(operators && input_addr && write_back))
    {
        if(!operators)
            panic("missing operators");
        if(!input_addr)
            panic("missing input_addr");
        if(!write_back)
            panic("missing write_back");
    }       
}

void check_instrs(struct array_node* cur)
{
    while(cur->next)
    {
        if(cur->val->type != OBJ)
            panic("one of the instr is not a object\n");
        check_instr(cur->val->obj);
        cur = cur->next;        
    }
}

void check_progarm(struct obj_node* obj)
{
    struct key_val_node* kv = obj->kv;
    if(strcmp(kv->key, "progarm") != 0)
        panic("first key -> val is not a progarm\n");
    if(obj->next->kv != 0)
        panic("should only exist a progarm in top level\n");
    
    check_instrs(obj->kv->val->arr);    
}

