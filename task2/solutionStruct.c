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
    /* The number of words which can be made. */
    int foundWordCount;
    /* The list of words that can be made. */
    char **words;

    /* Part B only */
    /* The number of letters which might follow. */
    int foundLetterCount;
    /* The list of letters. */
    char *followLetters;
};
