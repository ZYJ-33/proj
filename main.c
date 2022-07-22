#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include "./transi.h"
#include "./inner_node.h"
#include "./semantic/check.h"
#include "weight.h"
#include "./write_bin/write_bin.h"
#include "./config.h"


extern void print_node(struct basic_node*);
extern void yyparse();
struct basic_node* root;
extern FILE* yyin;

char weight_input[MAX_WEIGHT_FILE_LEN+1];
u_int32_t weight_file_len = 0;

void read_weight_file(char* weight_file)
{
    FILE* weight = fopen(weight_file, "r");
    if (weight == NULL)
    {
        printf("can't read weight_file %s", weight_file);
        exit(1);
    }
    weight_file_len = fread((void*)weight_input, 1, MAX_WEIGHT_FILE_LEN+1, weight);
    if(weight_file_len > MAX_WEIGHT_FILE_LEN)
    {
        printf("weight file too long, reset MAX_WEIGHT_FILE_LEN in config.h and recompile the project");
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        printf("usage : as progarm weight_file output_file\n");
        exit(1);
    }
    if((yyin = fopen(argv[1], "r")) == 0)
    {
        printf("can't open %s\n", argv[1]);
        return 1;
    }
    yyparse();
    struct obj_node* res = transit_obj(((struct two_child_node*)root));
    read_weight_file(argv[2]);

    init_weight(weight_input, weight_file_len);
    check_progarm(res);

    output_res();
    write_final_bin_to_file(argv[3]);
    return 0; 
    
}
