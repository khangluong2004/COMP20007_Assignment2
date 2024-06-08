/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
    Implementation for module which contains  
        Problem 1-related data structures and 
        functions.
    
    Sample solution implemented by Grady Fitzpatrick
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include "problem.h"
#include "problemStruct.c"
#include "solutionStruct.c"

/* MIN and MAX macros, to avoid having to define specific function for each type */
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

/* Number of words to allocate space for initially. */
#define INITIALWORDSALLOCATION 64

/* Denotes that the dimension has not yet been set. */
#define DIMENSION_UNSET (-1)

#define LDINFINITY (LDBL_MAX / 2.0L)

struct problem;
struct solution;

/* Sets up a solution for the given problem. */
struct solution *newSolution(struct problem *problem);

void readSequence(FILE *seqFile, int *seqLen, long double **seq);

/* Helper functions to malloc and free 2d matrix */
long double ** malloc2dMatrix(int row, int col);
void free2dMatrix(long double ** matrix, int row);

void readSequence(FILE *seqFile, int *seqLen, long double **seq){
    char *seqText = NULL;
    /* Read in text. */
    size_t allocated = 0;
    /* Exit if we read no characters or an error caught. */
    int success = getdelim(&seqText, &allocated, '\0', seqFile);

    if(success == -1){
        /* Encountered an error. */
        perror("Encountered error reading dictionary file");
        exit(EXIT_FAILURE);
    } else {
        /* Assume file contains at least one character. */
        assert(success > 0);
    }

    /* Progress through string. */
    int progress = 0;
    /* Table string length. */
    int seqTextLength = strlen(seqText);

    int commaCount = 0;
    /* Count how many numbers are present. */
    for(int i = 0; i < seqTextLength; i++){
        if(seqText[i] == ','){
            commaCount++;
        }
    }
    long double *seqLocal = (long double *) malloc(sizeof(long double) * (commaCount + 1));
    assert(seqLocal);

    int seqAdded = 0;
    while(progress < seqTextLength){
        int nextProgress;
        /* Read each value into the sequence. */
        assert(sscanf(seqText + progress, "%Lf , %n", &seqLocal[seqAdded], &nextProgress) == 1);
        assert(nextProgress > 0);
        progress += nextProgress;
        seqAdded++;
    }
    assert(seqAdded == (commaCount + 1));
    *seq = seqLocal;
    *seqLen = seqAdded;
    free(seqText);
}

/* 
    Reads the given dict file into a list of words 
    and the given board file into a nxn board.
*/
struct problem *readProblemA(FILE *seqAFile, FILE *seqBFile){
    struct problem *p = (struct problem *) malloc(sizeof(struct problem));
    assert(p);

    int seqALength = 0;
    long double *seqA = NULL;
    readSequence(seqAFile, &seqALength, &seqA);
    int seqBLength = 0;
    long double *seqB = NULL;
    readSequence(seqBFile, &seqBLength, &seqB);

    /* The length of the first sequence. */
    p->seqALength = seqALength;
    /* The first sequence. */
    p->sequenceA = seqA;

    /* The length of the second sequence. */
    p->seqBLength = seqBLength;
    /* The second sequence. */
    p->sequenceB = seqB;

    /* For Part D & F only. */
    p->windowSize = -1;
    p->maximumPathLength = -1;

    p->part = PART_A;

    return p;
}

struct problem *readProblemD(FILE *seqAFile, FILE *seqBFile, int windowSize){
    /* Fill in Part A sections. */
    struct problem *p = readProblemA(seqAFile, seqBFile);

    p->part = PART_D;
    p->windowSize = windowSize;

    return p;
}

struct problem *readProblemF(FILE *seqAFile, FILE *seqBFile, 
    int maxPathLength){
    /* Interpretation of inputs is same as Part A. */
    struct problem *p = readProblemA(seqAFile, seqBFile);
    
    p->part = PART_F;
    p->maximumPathLength = maxPathLength;

    return p;
}

/*
    Outputs the given solution to the given file. If colourMode is 1, the
    sentence in the problem is coloured with the given solution colours.
*/
void outputProblem(struct problem *problem, struct solution *solution, 
    FILE *outfileName){
    assert(solution);
    fprintf(outfileName, "%.2Lf\n", solution->optimalValue);
    switch(problem->part){
        case PART_A:
            assert(solution->matrix);
            for(int i = 1; i <= problem->seqALength; i++){
                for(int j = 1; j <= problem->seqBLength; j++){
                    if(solution->matrix[i][j] == LDINFINITY){
                        fprintf(outfileName, "    ");
                    } else {
                        fprintf(outfileName, "%.2Lf", solution->matrix[i][j]);
                    }
                    if(j < (problem->seqBLength)){
                        /* Intercalate with spaces. */
                        fprintf(outfileName, " ");
                    }
                }
                fprintf(outfileName, "\n");
            }
            break;
        case PART_D:
        case PART_F:
            break;
    }
}

/*
    Frees the given solution and all memory allocated for it.
*/
void freeSolution(struct solution *solution, struct problem *problem){
    if(solution){
        free2dMatrix(solution->matrix, problem->seqALength + 1);
        free(solution);
    }
}

/*
    Frees the given problem and all memory allocated for it.
*/
void freeProblem(struct problem *problem){
    if(problem){
        if(problem->sequenceA){
            free(problem->sequenceA);
        }
        if(problem->sequenceB){
            free(problem->sequenceB);
        }
        free(problem);
    }
}

/* Sets up a solution for the given problem */
struct solution *newSolution(struct problem *problem){
    struct solution *s = (struct solution *) malloc(sizeof(struct solution));
    assert(s);
    if(problem->part == PART_F){
        s->matrix = NULL;
    } else {
        s->matrix = malloc2dMatrix(problem->seqALength + 1, problem->seqBLength + 1);
    }

    s->optimalValue = -1;
    
    return s;
}

/*
    Solves the given problem according to Part A's definition
    and places the solution output into a returned solution value.
*/
struct solution *solveProblemA(struct problem *p){
    struct solution *s = newSolution(p);
    /* Fill in: Part A */
    // Initialize length of sequences
    int n = p->seqALength;
    int m = p->seqBLength;

    // Create and initialize the DTW matrix with infinity
    for (int i=0; i <= n; i++){
        for (int j=0; j <= m; j++){
            s->matrix[i][j] = LDINFINITY;
        }
    }

    s->matrix[0][0] = 0.0L;

    // Populate the DTW matrix
    for (int i=1; i <= n; i++){
        for (int j=1; j <= m; j++){
            long double cost = fabsl(p->sequenceA[i-1] - p->sequenceB[j-1]);
            s->matrix[i][j] = cost + MIN(s->matrix[i-1][j], 
                MIN(s->matrix[i][j-1], s->matrix[i-1][j-1]));
        }
    }

    // Put the optimal value in
    s->optimalValue = s->matrix[n][m]; 

    return s;
}

struct solution *solveProblemD(struct problem *p){
    struct solution *s = newSolution(p);
    /* Fill in: Part D */
    // Initialize length ofsequences
    int n = p->seqALength;
    int m = p->seqBLength;
    int windowSize = p->windowSize;

    // Create and initialize the DTW matrix with infinity
    for (int i=0; i <= n; i++){
        for (int j=0; j <= m; j++){
            s->matrix[i][j] = LDINFINITY;
        }
    }

    s->matrix[0][0] = 0.0L;

    // Populate the DTW matrix
    for (int i=1; i <= n; i++){
        // Check if i - windowSize is >= m (so the entire window is off the grid)
        if (i - windowSize > m){
            break;
        }

        // Otherwise, loop through the cells, but bounded to make sure 
        // only accessing the part of the window in the grid
        for (int j=MAX(i-windowSize, 1); j <= MIN(i+windowSize, m); j++){
            long double cost = fabsl(p->sequenceA[i-1] - p->sequenceB[j-1]);
            s->matrix[i][j] = cost + MIN(s->matrix[i-1][j], 
                MIN(s->matrix[i][j-1], s->matrix[i-1][j-1]));
        }
    }

    // Put the optimal value in
    s->optimalValue = s->matrix[n][m];

    return s;
}

/* Helper function to setup and free matrix, since matrix creation is turned off for F */
long double ** malloc2dMatrix(int row, int col){
    long double **matrix = (long double **) malloc(sizeof(long double *) * (row));
    assert(matrix);
    for(int i = 0; i < row; i++){
        matrix[i] = (long double *) malloc(sizeof(long double) * (col));
        assert(matrix[i]);
        for(int j = 0; j < col; j++){
            matrix[i][j] = 0;
        }
    }
    return matrix;
}

/* Helper function to free the matrix */
void free2dMatrix(long double ** matrix, int row){
    if(matrix){
        for(int i = 0; i < row; i++){
            free(matrix[i]);
        }
        free(matrix);
    }
}

/* Helper function to print the matrix for debugging */
void print2dMatrix(long double **matrix, int row, int col){
    printf("[");
    for (int i=0; i < row; i++){
        for (int j=0; j < col; j++){
            if (matrix[i][j] == LDINFINITY){
                printf("%LF, ", -1.0L);
                continue;
            }

            printf("%Lf, ", matrix[i][j]);
        }
        printf("EOL\n");
    }
    printf("]\n");
}

struct solution *solveProblemF(struct problem *p){
    /* Fill in: Part F */
    // Initialize sequence length and maxPathLength
    int n = p->seqALength;
    int m = p->seqBLength;
    int maxPathLength = p->maximumPathLength;


    struct solution *s = newSolution(p);
    s->optimalValue = LDINFINITY;

    // Create a cumulative cost matrix and  
    // another matrix to store the current path length 
    long double **cumCost = malloc2dMatrix(n + 1, m + 1);
    long double **pathLength = malloc2dMatrix(n + 1, m + 1);


    // Initialize the cum cost and the path length matrix
    for (int i=0; i <= n; i++){
        for (int j=0; j <= m; j++){
            cumCost[i][j] = LDINFINITY;
            pathLength[i][j] = -1;
        }
    }

    cumCost[0][0] = 0.0L;
    pathLength[0][0] = 0.0L;

    // Iterate through each incremental path length
    for (long double length=1; length <= maxPathLength; length++){
        // Traverse backward from the bottom-right to top-left
        // to ensure each entry from the previous pathLength
        // is not overwritten before its relevant neighbours are updated

        // This applies for both cumCost and pathLength. An example for pathLength is:
        // Eg: For [[0, 2, -1], 
        //          [2, 2, -1], 
        //          [-1, -1, -1]], 
        // the 2 in (1, 1) might be updated to 3 (if the path from the
        // nearby 2 is lower). Traversing left and up ensures that the 2 in (1, 1)
        // is used to update the path length of (1, 2), (2, 1) and (2, 2) to 3
        // first, before it is updated to 3 itself. 

        for (int i=n; i >= 1; i--){
            for (int j=m; j >= 1; j--){
                long double cost = fabsl(p->sequenceA[i-1] - p->sequenceB[j-1]);

                // The same recurrence relation is used, with the additional 
                // constraints that only cells that is at the end of the previous
                // path length is considered, ensuring the path length grows incrementally
                
                // Compute minimum addition (if any)
                // by checking if any of the top, left, top-left entry is the end of
                // the path of length - 1 (only consider previous path length cost
                // basically)
                long double minAddition = LDINFINITY;

                if (pathLength[i-1][j] == length - 1){
                    minAddition = MIN(minAddition, cumCost[i-1][j]);
                }

                if (pathLength[i][j-1] == length - 1){
                    minAddition = MIN(minAddition, cumCost[i][j-1]);
                }

                if (pathLength[i-1][j-1] == length - 1){
                    minAddition = MIN(minAddition, cumCost[i-1][j-1]);
                }

                // Update the cost if any of the cell is valid
                if (minAddition < LDINFINITY){
                    cumCost[i][j] = cost + minAddition;
                    pathLength[i][j] = length;
                }
            }
        }

        // Update the optimalValue in s for each path length
        s->optimalValue = MIN(s->optimalValue, cumCost[n][m]);
    }

    // Free the 2 matrices
    free2dMatrix(cumCost, n + 1);
    free2dMatrix(pathLength, n + 1);

    return s;
}

