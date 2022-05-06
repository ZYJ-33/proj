#include "ast.h"
#include "meta_type.h"
#include <stdlib.h>
#include <stdio.h>

struct basic_node* new_data_node(int type, char* data)
{
    struct data_node* dn = (struct data_node*) malloc(sizeof(*dn));
    dn->type = type;
    dn->data = data;
    dn->meta_type = META_DATA;
    return (struct basic_node*) dn;
}

struct basic_node* new_key_data_node(int type, char* key, char* data)
{
    struct key_data_node* kdn = (struct key_data_node*) malloc(sizeof(*kdn));
    kdn->type = type;
    kdn->key = key;
    kdn->data = data;
    kdn->meta_type = META_KEY_DATA;
    return (struct basic_node*) kdn;
}

struct basic_node* new_key_child_node(int type, char* key, struct basic_node* child)
{
    struct key_child_node* kcn = (struct key_child_node*) malloc(sizeof(*kcn));
    kcn->type = type;
    kcn->key = key;
    kcn->child = child;
    kcn->meta_type = META_KEY_CHILD;
    return (struct basic_node*)kcn;
}

struct basic_node* new_there_child_node(int type, struct basic_node* fst, struct basic_node* snd, struct basic_node* thr)
{
    struct there_child_node* tcn = (struct there_child_node*) malloc(sizeof(*tcn));
    tcn->type = type;
    tcn->fst = fst;
    tcn->snd = snd;
    tcn->thr = thr;
    tcn->meta_type = META_THERE_CHILD;
    return (struct basic_node*)tcn;
}

struct basic_node* new_two_child_node(int type, struct basic_node* fst, struct basic_node* snd)
{
    struct two_child_node* tcn = (struct two_child_node*)malloc(sizeof(*tcn));
    tcn->type = type;
    tcn->fst = fst;
    tcn->snd = snd;
    tcn->meta_type = META_TWO_CHILD;
    return (struct basic_node*)tcn;
}

struct basic_node* new_one_child_node(int type, struct basic_node* child)
{
    struct one_child_node* ocn = (struct one_child_node*)malloc(sizeof(*ocn));
    ocn->fst = child;
    ocn->type = type;
    ocn->meta_type = META_ONE_CHILD;
    return (struct basic_node*)ocn;
}

void travel(struct basic_node* node, void (*func)(struct basic_node*))
{
   if(node == 0)
          return;
   switch (node->meta_type)
   {
      case META_BASIC:
      {
            printf("reach a basic node\n");
            exit(1);
      }
      case META_DATA:
      {
            (*func)(node);
            break;
      }
      case META_KEY_DATA:
      {
            (*func)(node);
            break;
      }
      case META_KEY_CHILD:
      {
            (*func)(node);
            travel(((struct key_child_node*)node)->child, func);
            break;
      }
      case META_THERE_CHILD:
      {
            (*func)(node);
            struct there_child_node* n = (struct there_child_node*) node;
            travel(n->fst, func);
            travel(n->snd, func);
            travel(n->thr, func);
            break;
      }
      case META_TWO_CHILD:
      {
            (*func)(node);
            struct two_child_node* n = (struct two_child_node*)node;
            travel(n->fst, func);
            travel(n->snd, func);
            break;
      }
      case META_ONE_CHILD:
      {
            (*func)(node);
            struct one_child_node* n = (struct one_child_node*)node;
            travel(n->fst, func);
            break;
      }
      default:
      {
            printf("did not impl nodetype %d", node->meta_type);
            exit(1);
      }
   } 
}
