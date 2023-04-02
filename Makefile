compiler = gcc

interpreter:
	$(compiler) src/interpreter.c -o bf
	