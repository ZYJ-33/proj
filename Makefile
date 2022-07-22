AST_FILE := tree.tab.c
LEX_FILE := lex.yy.c
ALL_INPUT_FILE := weight.c ast.c inner_node.c main.c print_ast.c transi.c convert.c $(AST_FILE) $(LEX_FILE) semantic/check.c code_gen/code_gen.c write_bin/write_bin.c soft_operator/soft_operators.c

TEST_CASE := /home/ubuntu/sxdla/tests/test_conv_nhwc_int8/operators/conv_nhwc_int8_64
WEIGHT_FILE := weight_file
OUTPUT_FILE := output_test

$(AST_FILE):
	bison -d tree.y
	
$(LEX_FILE):
	flex lexer.l

weight_file:
	gcc ./ramdom_weight_file.c -o ran
	./ran
	rm ran

build: $(AST_FILE) $(LEX_FILE) weight_file
	gcc $(ALL_INPUT_FILE) -o as

gdb: $(AST_FILE) $(LEX_FILE) weight_file
	gcc $(ALL_INPUT_FILE) -g -o as

clean: 
	rm ./as ./$(WEIGHT_FILE) $(AST_FILE) $(LEX_FILE)

run: build
	./as $(TEST_CASE) $(WEIGHT_FILE) $(OUTPUT_FILE)

rebuild: clean build

regdb: clean gdb
