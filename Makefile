all: test test2
test: bigint.o bigint_helper.o main.o bigfloat_helper.o bigfloat.o
	gcc -o prog main.o bigint.o bigint_helper.o bigfloat_helper.o bigfloat.o -lm

test2: bigint.o bigint_helper.o test2.o bigfloat_helper.o bigfloat.o
	gcc -o test2 test2.o bigint.o bigint_helper.o bigfloat_helper.o bigfloat.o -lm

test2.o: test2.c
	gcc -c test2.c

main.o: main.c
	gcc -c main.c

bigint.o: bigint.c
	gcc -c bigint.c

bigint_helper.o: bigint_helper.c
	gcc -c bigint_helper.c

bigfloat.o: bigfloat.c
	gcc -c bigfloat.c

bigfloat_helper.o: bigfloat_helper.c
	gcc -c bigfloat_helper.c

clean: 
	rm -f *.o prog
