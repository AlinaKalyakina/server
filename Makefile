OUT=./garbage/
SRC=./src/
INC=./inc/
compile:
	mkdir -p $(OUT)
	gcc $(SRC)/main.c -I $(INC) -o $(OUT)/main.out
	gcc $(SRC)/test.c -I $(INC) -o $(OUT)/test.out

run: compile
	$(OUT)/main.out $(OUT)/test.out
