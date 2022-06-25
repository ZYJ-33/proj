#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include "./transi.h"
#include "./inner_node.h"
#include "./semantic/check.h"
#include "weight.h"
#include "./write_bin/write_bin.h"
#define LEN 500

extern void print_node(struct basic_node*);
extern void yyparse();
struct basic_node* root;
extern FILE* yyin;

int main(int argc, char* argv[])
{
    if(argc != 2)
            return 1;
    if((yyin = fopen(argv[1], "r")) == 0)
    {
        printf("can't open %s\n", argv[1]);
        return 1;
    }
    yyparse();
    
    char bytes[LEN];
    for(int i=0;i<LEN;i++)
           bytes[i] = i-LEN;

    struct obj_node* res = transit_obj(((struct two_child_node*)root));
    init_weight(bytes, LEN);
    check_progarm(res);
    
    output_res();    
    printf("semantic check pass\n");
    return 0; 
    
}
