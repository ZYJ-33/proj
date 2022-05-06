%{
#include "node.h"
#include "ast.h"
extern struct basic_node* root;
extern void yyerror(char* m);

%}

%union
{
    struct basic_node* t;
    char* str;
    int num;
    int boolean;
}

%token<str> STRING
%token<num> NUM
%token<boolean> BOOL
%token LEFT_BRACKET
%token RIGHT_BRACKET
%token COMMA
%token COLON
%token LEFT_BRACE
%token RIGHT_BRACE

%type<t> obj array pairs pair values value 

%%
start : obj {root = $1;}
      ;

obj : LEFT_BRACE pair pairs RIGHT_BRACE {$$ = new_two_child_node(OBJ_NODE, $2, $3);}
    | LEFT_BRACE RIGHT_BRACE{$$ = new_two_child_node(OBJ_NODE, 0, 0);}
    ;

array : LEFT_BRACKET value values RIGHT_BRACKET{$$ = new_two_child_node(ARRAY_NODE, $2, $3);}
      | LEFT_BRACKET RIGHT_BRACKET{$$ = new_two_child_node(ARRAY_NODE, 0, 0);}
      ;

pairs : COMMA pair pairs {$$ = new_two_child_node(PAIRS_NODE, $2, $3);}
      | {$$ = 0;}
      ;

pair  : STRING COLON value{$$ = new_key_child_node(PAIR_NODE, $1, $3);};

values : COMMA value values{$$ = new_two_child_node(VALUES_NODE, $2, $3);}
       | {$$ = 0;}
       ;

value : STRING {$$ = new_data_node(STRING_NODE,(char*)$1);}
      | NUM {$$ = new_data_node(NUM_NODE, $1);}
      | BOOL {$$ = new_data_node(BOOL_NODE, $1);}
      | obj {$$ = new_one_child_node(OBJ_VAL_NODE, $1);}
      | array{$$ = new_one_child_node(ARRAY_VAL_NODE, $1);}
        ;
%%
