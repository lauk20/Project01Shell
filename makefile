default: main.o commands.o
	gcc -o main main.o commands.o
main.o: main.c
	gcc -c main.c
commands.o: commands.c
	gcc -c commands.c
run:
	./main
