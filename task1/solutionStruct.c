/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
    Implementation for data structure used
        in storing the solution and its
        information.

    You may change this file if you would
        like to add additional fields.
*/
struct solution {
    /* The required (n + 1) x (m + 1) matrix. Only for Part A and D. */
    long double **matrix;
    /* The final optimal value (bottom-right value). */
    long double optimalValue;
};
