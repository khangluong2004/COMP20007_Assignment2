problem1a: problem1a.o problem.o 
	gcc -Wall -o problem1a problem1a.o problem.o -g -lm

problem1a.o: problem1a.c
	gcc -Wall -o problem1a.o -c problem1a.c -g

problem1d: problem1d.o problem.o 
	gcc -Wall -o problem1d problem1d.o problem.o -g -lm

problem1d.o: problem1d.c
	gcc -Wall -o problem1d.o -c problem1d.c -g

problem1f: problem1f.o problem.o 
	gcc -Wall -o problem1f problem1f.o problem.o -g -lm

problem1f.o: problem1f.c
	gcc -Wall -o problem1f.o -c problem1f.c -g

problem.o: problem.h problem.c solutionStruct.c problemStruct.c
	gcc -Wall -o problem.o -c problem.c -g
