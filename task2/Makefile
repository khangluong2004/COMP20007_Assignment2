problem2a.exe: problem2a.o problem.o prefixTree.o linkedList.o
	gcc -Wall -o problem2a.exe problem2a.o problem.o prefixTree.o linkedList.o -g

problem2a.o: problem2a.c
	gcc -Wall -o problem2a.o -c problem2a.c -g

problem2b.exe: problem2b.o problem.o prefixTree.o linkedList.o
	gcc -Wall -o problem2b.exe problem2b.o problem.o prefixTree.o linkedList.o -g

problem2b.o: problem2b.c
	gcc -Wall -o problem2b.o -c problem2b.c -g

problem2d.exe: problem2d.o problem.o prefixTree.o linkedList.o
	gcc -Wall -o problem2d.exe problem2d.o problem.o prefixTree.o linkedList.o -g

problem2d.o: problem2d.c
	gcc -Wall -o problem2d.o -c problem2d.c -g

problem.o: problem.h problem.c solutionStruct.c problemStruct.c
	gcc -Wall -o problem.o -c problem.c -g

prefixTree.o: prefixTree.c prefixTree.h
	gcc -Wall -o prefixTree.o -c prefixTree.c -g

linkedList.o: linkedList.c linkedList.h
	gcc -Wall -o linkedList.o -c linkedList.c -g 
