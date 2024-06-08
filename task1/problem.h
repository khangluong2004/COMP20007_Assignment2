/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
     Header for module which contains Problem 1-related 
        data structures and functions.
*/
#include <stdio.h>

struct problem;
struct solution;

/* 
    Reads the given sequence files and stores them.
*/
struct problem *readProblemA(FILE *seqAFile, FILE *seqBFile);

/* 
    Reads the given sequence files and stores them, as well as the window size.
*/
struct problem *readProblemD(FILE *seqAFile, FILE *seqBFile, int windowSize);

/*
    Same as Problem D, but part is set for Part F and maxPathLength is set 
    instead of windowSize.
*/
struct problem *readProblemF(FILE *seqAFile, FILE *seqBFile, int maxPathLength);

/*
    Solves the given problem according to Part A's definition
    and places the solution output into a returned solution value.
*/
struct solution *solveProblemA(struct problem *p);

/*
    Solves the given problem according to Part B's definition
    and places the solution output into a returned solution value.
*/
struct solution *solveProblemD(struct problem *p);

/*
    Solves the given problem according to Part D's definition
    and places the solution output into a returned solution value.
*/
struct solution *solveProblemF(struct problem *p);

/*
    Outputs the given solution to the given file.
*/
void outputProblem(struct problem *problem, struct solution *solution, 
    FILE *outfileName);

/*
    Frees the given solution and all memory allocated for it.
*/
void freeSolution(struct solution *solution, struct problem *problem);

/*
    Frees the given problem and all memory allocated for it.
*/
void freeProblem(struct problem *problem);

