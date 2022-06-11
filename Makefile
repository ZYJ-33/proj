AST_FILE := tree.tab.c
LEX_FILE := lex.yy.c
ALL_INPUT_FILE :=weight.c ast.c inner_node.c main.c print_ast.c transi.c convert.c $(AST_FILE) $(LEX_FILE) semantic/check.c code_gen/code_gen.c
TEST_CASE := simple_conv

$(AST_FILE):
	bison -d tree.y
	
$(LEX_FILE):
	flex lexer.l

build: $(AST_FILE) $(LEX_FILE)
	gcc $(ALL_INPUT_FILE) -o as

gdb: $(AST_FILE) $(LEX_FILE)
	gcc $(ALL_INPUT_FILE) -g -o as

clean: 
	rm ./as $(AST_FILE) $(LEX_FILE)

run: build
	./as $(TEST_CASE)

rebuild: clean build

regdb: clean gdb
