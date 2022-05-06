#include "ast.h"
#include "node.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
void print_node(struct basic_node* n);

void print_node(struct basic_node* n)
{
    switch (n->type) 
    {
        case OBJ_NODE:
        {
            printf("obj:\n");
            break;            
        }
        case ARRAY_NODE:
        {
            printf("array:\n");
            break;
        }
        case PAIRS_NODE:
        {
            printf("pairs:\n");
            break;
        }
        case PAIR_NODE:
        {
            struct key_child_node* kc = (struct key_child_node*)n;
            printf("%s:", kc->key);
            break;
        }
        case STRING_NODE:
        {
            struct data_node* dn = (struct data_node*) n;
            printf("%s\n", dn->data);    
            break;
        }
        case NUM_NODE:
        {
            struct data_node* dn = (struct data_node*) n;
            printf("%ld\n", (int64_t)dn->data);            
            break;
        }
        case BOOL_NODE:
        {
            struct data_node* dn = (struct data_node*) n;
            if((int64_t)dn->data == 1)
                printf("true\n");
            else
                printf("false\n");
            break;
        }
        case VALUES_NODE:
        {
            break; 
        }
        case OBJ_VAL_NODE:
        {
            break;
        }
        case ARRAY_VAL_NODE:
        {
            break;
        }
        default:
        {
            printf("unknow node found");
            exit(1);
        }
    }
}

