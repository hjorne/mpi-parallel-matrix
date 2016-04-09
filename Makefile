all: clcg4.h clcg4.c init.h init.c trans.h trans.c main.c add.h add.c data.h data.c collective.h collective.c groupio.h groupio.c
	gcc -I. -Wall -O3 -c clcg4.c -o clcg4.o
	mpicc -I. -Wall -O3 init.h init.c trans.h trans.c main.c add.h add.c data.h data.c collective.h collective.c groupio.h groupio.c clcg4.o -o assignment45 -lpthread
