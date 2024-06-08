/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
    Implementation for data structure used
        in storing the problem and its
        information.
*/

#ifndef PROBLEMPARTENUM_DEF
#define PROBLEMPARTENUM_DEF 1
enum problemPart {
    PART_A = 0,
    PART_D = 1,
    PART_F = 2
};
#endif

struct problem {
    /* The number of values in sequence A. */
    int seqALength;
    /* The numbers in sequence A. */
    long double *sequenceA;

    /* The number of values in sequence B. */
    int seqBLength;
    /* The numbers in sequence B. */
    long double *sequenceB;

    /* For Part D only, the window size. */
    int windowSize;

    /* For Part F only, the maximum path length. */
    int maximumPathLength;

    /* Which problem part is being solved. */
    enum problemPart part;
};


