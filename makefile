default: main.o commands.o commands.h
	gcc -o main main.o commands.o
main.o: main.c commands.h
	gcc -c main.c
commands.o: commands.c commands.h
	gcc -c commands.c
run:
	./main
