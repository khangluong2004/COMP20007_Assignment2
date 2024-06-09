/*
    Written by Grady Fitzpatrick for 
    COMP20007 Assignment 2 2024 Semester 1
    
    Implementation for module which contains  
        Problem 2-related data structures and 
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
#include "problem.h"
#include "problemStruct.c"
#include "solutionStruct.c"
#include "prefixTree.h"
#include "linkedList.h"

/* Number of words to allocate space for initially. */
#define INITIALWORDSALLOCATION 64

/* Denotes that the dimension has not yet been set. */
#define DIMENSION_UNSET (-1)

/*MIN and MAX macros*/
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

struct problem;
struct solution;

/* Define getdelim since the method is not available for my compiler.
Doesn't support multi-threading */
size_t getdelim(char **lineptr, size_t *n, int delim, FILE *stream){
    if (lineptr == NULL || n == NULL || stream == NULL){
      return -1;
    }

    int length = 0;
    char c;
    // Get the length for malloc
    while ((c = fgetc(stream)) != delim){
        if (c == EOF){
            break;
        }
        length++;
    }

    char* string = (char *) malloc(sizeof(char) * (length + 1));
    
    // Move the pointer back to scan the string
    fseek(stream, -length, SEEK_CUR);
    for (int i=0; i < length; i++){
        string[i] = fgetc(stream);
    }
    string[length] = '\0';

    *lineptr = string;
    *n = length;

    return length;
}

/* Sets up a solution for the given problem. */
struct solution *newSolution(struct problem *problem);

/* 
    Reads the given dict file into a list of words 
    and the given board file into a nxn board.
*/
struct problem *readProblemA(FILE *dictFile, FILE *boardFile){
    struct problem *p = (struct problem *) malloc(sizeof(struct problem));
    assert(p);

    /* Part B onwards so set as empty. */
    p->partialString = NULL;

    int wordCount = 0;
    int wordAllocated = 0;
    char *dictText = NULL;
    char **words = NULL;

    /* Read in text. */
    size_t allocated = 0;
    /* Exit if we read no characters or an error caught. */
    int success = getdelim(&dictText, &allocated, '\0', dictFile);

    if(success == -1){
        /* Encountered an error. */
        perror("Encountered error reading dictionary file");
        exit(EXIT_FAILURE);
    } else {
        /* Assume file contains at least one character. */
        assert(success > 0);
    }

    char *boardText = NULL;
    /* Reset allocated marker. */
    allocated = 0;
    success = getdelim(&boardText, &allocated, '\0', boardFile);

    if(success == -1){
        /* Encountered an error. */
        perror("Encountered error reading board file");
        exit(EXIT_FAILURE);
    } else {
        /* Assume file contains at least one character. */
        assert(success > 0);
    }

    /* Progress through string. */
    int progress = 0;
    /* Table string length. */
    int dictTextLength = strlen(dictText);

    /* Count words present. */
    while(progress < dictTextLength){
        char *word = NULL;
        int nextProgress;
        /* Read each value into the dictionary. */
        if(progress == 0){
            /* First line. */
            int wordNumberGuess;
            assert(sscanf(dictText + progress, "%d %n", &wordNumberGuess, &nextProgress) == 1);
            /* Will fail if integer missing from the start of the words. */
            assert(nextProgress > 0);
            if(wordNumberGuess > 0){
                wordAllocated = wordNumberGuess;
                words = (char **) malloc(sizeof(char *) * wordAllocated);
                assert(words);
            }
            progress += nextProgress;
            continue;
        } else {
            assert(sscanf(dictText + progress, "%m[^\n] %n", &word, &nextProgress) == 1);
            assert(nextProgress > 0);
            progress += nextProgress;
        }

        /* Check if more space is needed to store the word. */
        if(wordAllocated <= 0){
            words = (char **) malloc(sizeof(char *) * INITIALWORDSALLOCATION);
            assert(words);
            wordAllocated = INITIALWORDSALLOCATION;
        } else if(wordCount >= wordAllocated){
            words = (char **) realloc(words, sizeof(char *) * 
                wordAllocated * 2);
            assert(words);
            wordAllocated = wordAllocated * 2;
            /* Something has gone wrong if there's not sufficient word 
                space for another word. */
            assert(wordAllocated > wordCount);
        }

        words[wordCount] = word;
        wordCount++;
    }
    
    /* Done with dictText */
    if(dictText){
        free(dictText);
    }
    
    /* Now read in board */
    progress = 0;
    int dimension = 0;
    int boardTextLength = strlen(boardText);
    /* Count dimension with first line */
    while(progress < boardTextLength){
        /* Count how many non-space characters appear in line. */
        if(boardText[progress] == '\n' || boardText[progress] == '\0'){
            /* Reached end of line. */
            break;
        }
        if(isalpha(boardText[progress])){
            dimension++;
        }
        progress++;
    }

    assert(dimension > 0);

    /* Check each line has the correct dimension. */
    for(int i = 1; i < dimension; i++){
        int rowDim = 0;
        if(boardText[progress] == '\n'){
            progress++;
        }
        while(progress < boardTextLength){
            /* Count how many non-space characters appear in line. */
            if(boardText[progress] == '\n' || boardText[progress] == '\0'){
                /* Reached end of line. */
                break;
            }
            if(isalpha(boardText[progress])){
                rowDim++;
            }
            progress++;
        }
        if(rowDim != dimension){
            fprintf(stderr, "Row #%d had %d letters, different to Row #1's %d letters.\n", i + 1, rowDim, dimension);
            assert(rowDim == dimension);
        }
    }

    /* Define board. */
    char *boardFlat = (char *) malloc(sizeof(char) * dimension * dimension);
    assert(boardFlat);
    
    /* Reset progress. */
    progress = 0;
    for(int i = 0; i < dimension; i++){
        for(int j = 0; j < dimension; j++){
            int nextProgress;
            assert(sscanf(boardText + progress, "%c %n", &boardFlat[i * dimension + j], &nextProgress) == 1);
            progress += nextProgress;
        }
    }

    char **board = (char **) malloc(sizeof(char **) * dimension);
    assert(board);
    for(int i = 0; i < dimension; i++){
        board[i] = &boardFlat[i * dimension];
    }

    // fprintf(stderr, "\n");

    // Free boardText
    if (boardText){
        free(boardText);
    }

    /* The number of words in the text. */
    p->wordCount = wordCount;
    /* The list of words in the dictionary. */
    p->words = words;

    /* The dimension of the board (number of rows) */
    p->dimension = dimension;

    /* The board, represented both as a 1-D list and a 2-D list */
    p->boardFlat = boardFlat;
    p->board = board;

    /* For Part B only. */
    p->partialString = NULL;

    p->part = PART_A;

    return p;
}

struct problem *readProblemB(FILE *dictFile, FILE *boardFile, 
    FILE *partialStringFile){
    /* Fill in Part A sections. */
    struct problem *p = readProblemA(dictFile, boardFile);

    char *partialString = NULL;

    /* Part B has a string that is partially given - we assume it follows 
        word conventions for the %s specifier. */
    assert(fscanf(partialStringFile, "%ms", &partialString) == 1);
    
    p->part = PART_B;
    p->partialString = partialString;

    return p;
}

struct problem *readProblemD(FILE *dictFile, FILE *boardFile){
    /* Interpretation of inputs is same as Part A. */
    struct problem *p = readProblemA(dictFile, boardFile);
    
    p->part = PART_D;
    return p;
}

/*
    Outputs the given solution to the given file. If colourMode is 1, the
    sentence in the problem is coloured with the given solution colours.
*/
void outputProblem(struct problem *problem, struct solution *solution, 
    FILE *outfileName){
    assert(solution);
    switch(problem->part){
        case PART_A:
        case PART_D:
            assert(solution->foundWordCount == 0 || solution->words);
            for(int i = 0; i < solution->foundWordCount; i++){
                fprintf(outfileName, "%s\n", solution->words[i]);
            }
            break;
        case PART_B:
            assert(solution->foundLetterCount == 0 || solution->followLetters);
            for(int i = 0; i < solution->foundLetterCount; i++){
                if(isalpha(solution->followLetters[i])){
                    fprintf(outfileName, "%c\n", solution->followLetters[i]);
                } else {
                    fprintf(outfileName, " \n");
                }
            }
            break;
    }
}

/*
    Frees the given solution and all memory allocated for it.
*/
void freeSolution(struct solution *solution, struct problem *problem){
    if(solution){
        if(solution->followLetters){
            free(solution->followLetters);
        }

        if(solution->words){
            // Free each word first
            for (int i=0; i < solution->foundWordCount; i++){
                free(solution->words[i]);
            }
            free(solution->words);
        }
        free(solution);
    }
}

/*
    Frees the given problem and all memory allocated for it.
*/
void freeProblem(struct problem *problem){
    if(problem){
        if(problem->words){
            for(int i = 0; i < problem->wordCount; i++){
                if(problem->words[i]){
                    free(problem->words[i]);
                }
            }
            free(problem->words);
        }
        if(problem->board){
            free(problem->board);
        }
        if(problem->boardFlat){
            free(problem->boardFlat);
        }
        if(problem->partialString){
            free(problem->partialString);
        }
        free(problem);
    }
}

/* Sets up a solution for the given problem */
struct solution *newSolution(struct problem *problem){
    struct solution *s = (struct solution *) malloc(sizeof(struct solution));
    assert(s);
    s->foundWordCount = 0;
    s->words = NULL;
    s->foundLetterCount = 0;
    s->followLetters = NULL;
    
    return s;
}

/* Helper function to setup and free matrix, since matrix creation is turned off for F for some reason */
int ** malloc2dMatrix(int row, int col){
    int **matrix = (int **) malloc(sizeof(int *) * (row));
    assert(matrix);
    for(int i = 0; i < row; i++){
        matrix[i] = (int *) malloc(sizeof(int) * (col));
        assert(matrix[i]);
        for(int j = 0; j < col; j++){
            matrix[i][j] = 0;
        }
    }
    return matrix;
}

/* Helper function to free the matrix */
void free2dMatrix(int ** matrix, int row){
    if(matrix){
        for(int i = 0; i < row; i++){
            free(matrix[i]);
        }
        free(matrix);
    }
}


/*
    Parse found words into the list of words inside the solutions, and
    sort it ordered alphabetically and break ties by length
*/

/*
    Helper function to parse the tree and get the found words.
    Since the tree is parsed from smallest to largest letter at each level, this would ensure
    all the words are sorted.

    The tree is marked along the path to all the encountered words, so ignore 
    all the unmarked paths, which can reduce the traversal significantly
*/
void parseTreeFoundWords(struct prefixTree* prefixTree, struct solution *s, int* sIndex, 
    struct problem *p){
    
    // Stop if the tree is null
    if (prefixTree == NULL){
        return;
    }

    // Stop if the current branch is not marked
    // since this indicates this path is not used for found words
    if (prefixTree->visited == UNMARKED){
        return;
    }

    // If found the end of a path
    if (prefixTree->child[(int) '\0'] != NULL && prefixTree->child[(int) '\0']->visited == MARKED){
        int index = prefixTree->child[(int) '\0']->dictionaryIndex;

        // Copy the found word over to the final result
        int stringLen = strlen(p->words[index]);
        s->words[*sIndex] = (char *) malloc(sizeof(char) * (stringLen + 1));
        assert(s->words[*sIndex]);
        strncpy(s->words[*sIndex], p->words[index], stringLen + 1);
        *sIndex += 1;
    }

    // Otherwise, continue traversing downward, from the smallest to largest character,
    // ignoring NULL character, so start at 1
    for (int i=1; i < CHILD_COUNT; i++){
        parseTreeFoundWords(prefixTree->child[i], s, sIndex, p);
    }
}

void parseFoundWords(struct prefixTree* prefixTree, struct solution *s, struct problem *p){
    // Return early if found nothing
    if (s->foundWordCount <= 0){
        return;
    }

    s->words = (char **) malloc(sizeof(char *) * s->foundWordCount);
    assert(s->words);

    // Initialize the values of the s->words
    for (int i=0; i < s->foundWordCount; i++){
        s->words[i] = NULL;
    }

    int sIndex = 0;
    
    // No sorting, so parse the tree again for the actual word
    parseTreeFoundWords(prefixTree, s, &sIndex, p);
}

/*
    Helper function for printing out a list during debugging
*/
void printList(int *arr, int n){
    printf("[");
    for (int i=0; i < n; i++){
        printf("%d, ", arr[i]);
    }
    printf("]");
}

/*
    TraverseHelper for A: 
    Helper functions to traverse through the board and tree for
    given word. Split out to allow easier recursion.

    Return MARKED if found a word in its children (or itself)
*/
int problemATraverse(struct problem *p, struct prefixTree *prefixTree, 
    struct solution *s, int** visited, 
    int curRow, int curCol){
    int isMarked = UNMARKED;
    
    // Check if found a word. If yes, and not marked, then marked it, and update the count
    if (prefixTree->child[(int) '\0'] != NULL && 
        prefixTree->child[(int) '\0']->visited == UNMARKED){
        prefixTree->child[(int) '\0']->visited = MARKED;
        s->foundWordCount += 1;

        // Mark the parents as well
        prefixTree->visited = MARKED;
        isMarked = MARKED;
    }

    // Guard against going out of the board
    if (curRow < 0 || curRow >= p->dimension || curCol < 0 || curCol >= p->dimension){
        return isMarked;
    }

    // Check if already visited
    if (visited[curRow][curCol] == 1){
        return isMarked;
    }

    // Check if the current character in the board matches any child of the tree
    // Else, return
    int curChar = (int) tolower(p->board[curRow][curCol]);
    if (prefixTree->child[curChar] == NULL){
        return isMarked;
    }

    // Found a child, so mark visited and start recursively call on all 8 directions
    struct prefixTree *nextTree = prefixTree->child[curChar];
    visited[curRow][curCol] = 1;

    for (int nextCol = curCol - 1; nextCol <= curCol + 1; nextCol++){
        for (int nextRow = curRow - 1; nextRow <= curRow + 1; nextRow++){
            // Skip the current node
            if (nextCol == curCol && nextRow == curRow){
                continue;
            }

            // Call traverse on valid 8 locations
            int isChildMarked = problemATraverse(p, nextTree, s, visited, nextRow, nextCol);
            isMarked = isMarked || isChildMarked;
        }
    }

    // Backtrack: Unmark visited to allow a new path visiting the node
    visited[curRow][curCol] = 0;

    // Mark the tree from unmarked to marked
    // Don't update if marked as MARKED, since this node
    // might be required for some other path to found words, even if the current path
    // don't need it
    if (prefixTree->visited == UNMARKED && isMarked){
        prefixTree->visited = isMarked;
    }

    return isMarked;
}


/*
    Refactor the processing logic from the prefixTree intialization logic,
    as part D used the same set up for parsing found words, solution and
    checking visited, with a difference in the traversing and the tree.

    The function processing includes traversing the board, match string,
    and parse the matched strings to the correct solution format.

    The traverse function is parameterized, along with an adjacency matrix so that
    this "set up" code can be re-used in part D. The adjacencyList is null for part A. 
*/

void processingLogicProblemA(struct problem *p, struct solution *s, struct prefixTree *prefixTree){
    // Initialize visited and the found array
    int dimension = p->dimension;
    int **visited = malloc2dMatrix(dimension, dimension);

    for (int i=0; i < dimension; i++){
        for (int j=0; j < dimension; j++){
            visited[i][j] = 0;
        }
    }

    // Start path finding on all starting position on the board
    for (int i=0; i < dimension; i++){
        for (int j=0; j < dimension; j++){
            problemATraverse(p, prefixTree, s, visited, i, j);
        }
    }

    // Convert the found array into the solution fomat required
    parseFoundWords(prefixTree, s, p);

    // Free used matrix
    free2dMatrix(visited, dimension);
}


/*
    Solves the given problem according to Part A's definition
    and places the solution output into a returned solution value.
*/
struct solution *solveProblemA(struct problem *p){
    struct solution *s = newSolution(p);
    s->foundWordCount = 0;
    
    // Create a new prefix tree
    struct prefixTree* prefixTree = newPrefixTree();

    // Add all the world in dictionary into the tree
    int wordCount = p->wordCount;
    for (int i=0; i < wordCount; i++){
        prefixTree = addWordToTree(prefixTree, p->words[i], i);
    }

    // Run the processing logic, which includes traversing the board, match string,
    // and parse the matched strings to the correct solution format
    processingLogicProblemA(p, s, prefixTree);

    // Free the tree when finished
    freePrefixTree(prefixTree);
    return s;
}

/*
    PART B
*/

/*
    Helper function to find the possible next chars from the dictionary.
    Return the count of possible characters
*/
int findNextChars(char* partialString, struct prefixTree* prefixTree, int* nextLetters){
    struct prefixTree* current = prefixTree;
    int position = 0;
    while (partialString[position] != '\0'){
        // If can't find the partial string in the dictionary, return early
        if (current->child[(int) partialString[position]] == NULL){
            return 0;
        }

        // Otherwise, continue going down the prefixTree
        current = current->child[(int) partialString[position]];
        // Increment the position
        position++;
    }

    // If reached this point, the current node's child will represent the possible next letters
    // So copy it to the nextLetters array

    // Flag to check if there is any child
    int count = 0;
    for (int i=0; i < CHILD_COUNT; i++){
        if (current->child[i] != NULL){
            nextLetters[i]++;
            count++;
        }
    }

    return count;
}

/*
    Helper function to traverse the board
*/
void problemBTraverse(struct problem *p, int strLength, int strPosition,
int* nextLetters, int** visited, int curRow, int curCol){
    char* partialString = p->partialString;

    // Guard against going out of the board
    if (curRow < 0 || curRow >= p->dimension || curCol < 0 || curCol >= p->dimension){
        return;
    }

    // Check if already visited
    if (visited[curRow][curCol] == 1){
        return;
    }

    int curChar = tolower(p->board[curRow][curCol]);

    // Check if reaching the nextLetters and it's in the dictionary, 
    // then update nextLetters and return
    if (strPosition >= strLength && nextLetters[curChar] == 1){
        nextLetters[curChar]++;
        return;
    }

    // Else, check if match the partialString
    if (curChar != partialString[strPosition]){
        return;
    }

    // If match, mark visited and recursively call on all 8 directions
    visited[curRow][curCol] = 1;

    for (int nextCol = curCol - 1; nextCol <= curCol + 1; nextCol++){
        for (int nextRow = curRow - 1; nextRow <= curRow + 1; nextRow++){
            // Skip the current node
            if (nextCol == curCol && nextRow == curRow){
                continue;
            }

            // Call traverse on valid 8 locations
            problemBTraverse(p, strLength, strPosition + 1, nextLetters, visited, nextRow, nextCol);
        }
    }

    // Backtrack: Unmark visited to allow other path to reuse the cell
    visited[curRow][curCol] = 0;
    return;
}

struct solution *solveProblemB(struct problem *p){
    struct solution *s = newSolution(p);
    s->foundLetterCount = 0;
    s->followLetters = NULL;

    // Create a new prefixTree
    struct prefixTree* prefixTree = newPrefixTree();

    // Add all the world in dictionary into the tree
    int wordCount = p->wordCount;
    for (int i=0; i < wordCount; i++){
        prefixTree = addWordToTree(prefixTree, p->words[i], i);
    }

    // Find possible next letters at the end of the partial string from the dictionary
    int* nextLetters = (int *) malloc(sizeof(int) * CHILD_COUNT);
    assert(nextLetters);
    for (int i=0; i < CHILD_COUNT; i++){
        nextLetters[i] = 0;
    }

    int count = findNextChars(p->partialString, prefixTree, nextLetters);

    // If the path is not found in the dictionary or no children for nextLetters, return early
    if (count == 0){
        free(nextLetters);
        freePrefixTree(prefixTree);
        return s;
    }

    // Now, update the nextLetters result by traversing the board to see which letter is possible
    int dimension = p->dimension;
    int **visited = malloc2dMatrix(dimension, dimension);

    for (int i=0; i < dimension; i++){
        for (int j=0; j < dimension; j++){
            visited[i][j] = 0;
        }
    }

    // Compute the strLength once for efficiency
    int strLength = strlen(p->partialString);

    for (int i=0; i < dimension; i++){
        for (int j=0; j < dimension; j++){
            problemBTraverse(p, strLength, 0, nextLetters, visited, i, j);
        }
    }

    // Now, if the nextLetters occur both in the dictionary and the board, the count would be 2
    // Except for null, which only need 1 (since it doesn't appear on the board)
    for (int i=0; i < CHILD_COUNT; i++){
        if (nextLetters[i] == 2){
            s->foundLetterCount++;
        }
    }

    // Check the edge case for NULL value
    if (nextLetters[(int) '\0'] >= 1){
        s->foundLetterCount++;
    }

    // Check if there is overlapping followLetters
    if (s->foundLetterCount != 0){
        // Initialize the array for the followLetters, 
        // and place the correct symbol in the array
        s->followLetters = (char *) malloc(sizeof(char) * s->foundLetterCount);
        assert(s->followLetters);
        int sIndex = 0;

        // Add the null in, if any
        if (nextLetters[(int) '\0'] >= 1){
            s->followLetters[0] = ' ';
            sIndex++;
        }

        // Check the other letters
        for (int i=1; i < CHILD_COUNT; i++){
            // If count 2, it means the letter appear on both the board and dictionary
            if (nextLetters[i] >= 2){
                if (isalpha(i)){
                    s->followLetters[sIndex] = (char) i;
                } else {
                    s->followLetters[sIndex] = ' ';
                }

                sIndex++;
            }
        }
    }

    // Free all used arrays/ structures
    free(nextLetters);
    free2dMatrix(visited, dimension);
    freePrefixTree(prefixTree);

    return s;
}

/*
    Part D:
    Main optimization:
    1. Filter words with unique characters to add to the prefix tree only
    2. Compress the board graph by merging cluster of the same letter to 1 single node

    The first optimization is to pre-filter invalid words upfront, 
    reducing the size of the prefix tree and valid paths

    The second optimization is to abuse the fact that all word can only
    have unique characters, thus we can fuse "adjacent" children/cells with the same
    character into 1 cell/ node, thus further pruning the path size and
    speed up those inputs with large-sized cluster
*/


/*
    Helper function to check if the word only contains non-repeated characters
    For first optimization
*/
int checkRepeatedCharacters(char* word){
    // Create an array to store the count for each character
    int characterCount[CHILD_COUNT] = {0};
    int wordLength = strlen(word);

    for (int i=0; i < wordLength; i++){
        int curChar = word[i];
        characterCount[curChar]++;

        // If see a character twice, then return false (0)
        if (characterCount[curChar] >= 2){
            return 0;
        }
    }

    return 1;
}

/*
    Second optimization
*/
/*
    Define the board as a graph, with each node labelled by (row * dimension + col).
    The children of the node is all the nodes that can be reached by that node.
    Initially each node has max 8 children for the 8 adjacent cells.

    Compress the board, fuse adjacent children node with the same letter together into
    1 single primaryCell and traverse down that cell.

    Can't simply fuse all nodes, since leading to "teleportation" issue between nodes,
    since for a specific parents, reacheable and unreachable node that shares the same
    characters are treated as the same thing.

    Thus, only fuse the children of a nodes, and
    need to de-fuse those nodes after finishing traversing the group.

    Further details are explained in part C.
*/

/*
    Each cell[i][j] on the board is represented uniquely by the 
    node numbered (i * dim + j)
    
    Helper functions: To convert a cell location to its unique label, 
    and reverse the label back to the cell location
*/
void convertToLabel(int row, int col, int dimension, int* label){
    *label = (row * dimension) + col;
}

void convertToLocation(int label, int dimension, int* row, int* col){
    *row = label/ dimension;
    *col = label % dimension;
}


/*
    Build an adjacency list with the same labelling
    rule as specified above
*/


/*
    Helper function: Add the initial adjacent cells on the board as children to a node/cell
*/
void buildAdjacencyListHelper(struct problem *p, list_t** adjacencyList, int curRow, int curCol){
    int dimension = p->dimension;
    int curLabel = -1;
    convertToLabel(curRow, curCol, dimension, &curLabel);

    // Add edges to all 8 directions
    for (int nextCol = MAX(curCol - 1, 0); nextCol <= MIN(curCol + 1, dimension - 1); nextCol++){
        for (int nextRow = MAX(curRow - 1, 0); nextRow <= MIN(curRow + 1, dimension - 1); nextRow++){
            // Skip the current node to avoid self edge
            if (nextCol == curCol && nextRow == curRow){
                continue;
            }

            int nextLabel = -1;
            convertToLabel(nextRow, nextCol, dimension, &nextLabel);
            // Only do TO (and not FROM) direction to avoid duplicate
            prependNode(adjacencyList[curLabel], nextLabel);
        }
    }
}

/*
    Build the adjacencyList graph representation
*/
list_t ** buildAdjacencyList(struct problem *p){
    int dimension = p->dimension;
    list_t** adjacencyList = (list_t**) malloc((dimension * dimension) * sizeof(list_t *));
    assert(adjacencyList);

    // Initialize adjacencyList
    for (int i=0; i < dimension * dimension; i++){
        adjacencyList[i] = createList();
        assert(adjacencyList[i]);
    }

    // Initialize edge to all cells on the board
    for (int i=0; i < dimension; i++){
        for (int j=0; j < dimension; j++){
            buildAdjacencyListHelper(p, adjacencyList, i, j);
        }
    }

    return adjacencyList;
}

/*
    Free the adjacencyList
*/
void freeAdjacencyList(list_t** adjacencyList, struct problem *p){
    int dimension = p->dimension;
    for (int i=0; i < dimension * dimension; i++){
        freeList(adjacencyList[i]);
    }
    free(adjacencyList);
}

/*
    MAIN TRAVERSING PART
*/


/*
    Helper function: Compress the cell on the adjacencyList.
    Compress the children from the node label2 into the children of the node label1
    (if the children is not already included, checked using the combineArray).
*/
void compressCellAdjacencyList(list_t** adjacencyList, int* combineArray, int dimension, int label1, int label2){
    // Traverse the children of label2
    node_t* label2Current = adjacencyList[label2]->root;
    while (label2Current != NULL){
        int currentChildLabel = label2Current->value;
        // Check if already included in label1
        if (combineArray[currentChildLabel] == 0){
            combineArray[currentChildLabel] = 1;
            prependNode(adjacencyList[label1], currentChildLabel);
        }
        label2Current = label2Current->next;
    }
}

/*
    Helper function to mark and compress the fused children of all node within the same group (same characters
    and share the current parent)
*/
void compressFusedCell(struct problem *p, list_t** adjacencyList, int* combineArray,
    node_t* listRoot, node_t* primaryCell){
    
    int dimension = p->dimension;
    // Reset combine array
    memset(combineArray, 0, sizeof(int) * (dimension * dimension));

    // Set the combine array to contain the children of the primaryCell, since they are already in
    // the list, thus no need to add
    node_t* primaryCurrent = adjacencyList[primaryCell->value]->root;
    while (primaryCurrent != NULL){
        combineArray[primaryCurrent->value] = 1;
        primaryCurrent = primaryCurrent->next;
    }
 
    // Fuse the entire group into the primaryCell
    node_t* current = listRoot;
    while (current != NULL){
        compressCellAdjacencyList(adjacencyList, combineArray, dimension, primaryCell->value, current->value);
        current = current->next;
    }
}

/*
    Helper function to revert a child list of a node "label" back to the original list
*/
void decompressCellAdjacencyList(struct problem* p, list_t** adjacencyList, int label){
    // Free the entire current list
    freeList(adjacencyList[label]);

    // Create a new children list using the neighbour of the label
    adjacencyList[label] = createList();
    int row = -1;
    int col = -1;
    convertToLocation(label, p->dimension, &row, &col);
    buildAdjacencyListHelper(p, adjacencyList, row, col);
}

/*
    Helper function to decompress fused cells after we finish traversing the subtree of those cells.
    Decompress is crucial, as only all the CHILDREN with the same character can only be considered
    as 1 single cell by the current PARENT node, and still need to be treated as separate cells by all other nodes.

    This is done simply by just reconstruct the initial list from the surrounding 8 cells

    Since the fusing/compressing only happens on the primary cell, so only need to decompress that cell
*/

void decompressFusedCell(struct problem *p, list_t** adjacencyList, node_t* primaryCell){
    decompressCellAdjacencyList(p, adjacencyList, primaryCell->value);
}

/*
    Helper function to process the grouping of child node into groups with
    same character (only considering those in the prefixTree)
*/
void groupSameCharacters(struct problem *p, struct prefixTree* nextTree, list_t* childGroup[CHILD_COUNT], list_t* curChildren){
    int dimension = p->dimension;
    node_t* currentChild = curChildren->root;
    // Iterate through the children for grouping
    while (currentChild != NULL){
        int childRow = -1;
        int childCol = -1;
        convertToLocation(currentChild->value, dimension, &childRow, &childCol);
        int childChar = (int) p->board[childRow][childCol];

        // Ignore if the character not in the tree
        if (nextTree->child[tolower(childChar)] == NULL){
            currentChild = currentChild->next;
            continue;
        }

        // Else, add it to the grouping
        if (childGroup[childChar] == NULL){
            childGroup[childChar] = createList();
            assert(childGroup[childChar]);
        }

        prependNode(childGroup[childChar], currentChild->value);
        currentChild = currentChild->next;
    }
}

/*
    2 recursive functions used below, so need to specify their signature here
*/
int problemDTraverse(struct problem *p, struct prefixTree *prefixTree, 
    list_t** adjacencyList, int* combineArray,
    struct solution *s, int** visited, 
    int curRow, int curCol);

int processCharacterGroup(list_t* curGroup, struct problem *p, struct prefixTree *nextTree, 
    list_t** adjacencyList, int* combineArray, struct solution *s, int** visited);


/*
    Helper function: Process character grouping,
    include select a primaryCell, compress the list, call the traversal 
    with the primaryCell, and then decompress the list
*/

int processCharacterGroup(list_t* curGroup, struct problem *p, struct prefixTree *nextTree, 
    list_t** adjacencyList, int* combineArray, struct solution *s, int** visited){
    int isChildMarked = UNMARKED;
    int dimension = p->dimension;
    // Skip empty location
    if (curGroup == NULL){
        return UNMARKED;
    }

    // Else, rip off the root to be the primary cell for that group
    // Then compress everything else to the primaryCell
    node_t* primaryCell = curGroup->root;
    compressFusedCell(p, adjacencyList, combineArray, primaryCell->next, primaryCell);

    // Then recursively traverse down the primaryCell
    int primaryRow = -1;
    int primaryCol = -1;
    convertToLocation(primaryCell->value, dimension, &primaryRow, &primaryCol);
    isChildMarked = problemDTraverse(p, nextTree, adjacencyList, combineArray, s, visited, primaryRow, primaryCol);

    // Then uncompress the primaryCell
    decompressFusedCell(p, adjacencyList, primaryCell);
    return isChildMarked;
}


/*
    Apply the 2 optimization on the tree and on the graph.
    Then, backtrack on the reduced graph and tree using similar backtracking
    method like A, with the only difference is the next cell/node is determined
    on the compressed adjacencyList, and not directly on the board
*/
int problemDTraverse(struct problem *p, struct prefixTree *prefixTree, 
    list_t** adjacencyList, int* combineArray,
    struct solution *s, int** visited, 
    int curRow, int curCol){

    int isMarked = UNMARKED;

    // Guard against going out of the board
    if (curRow < 0 || curRow >= p->dimension || curCol < 0 || curCol >= p->dimension){
        return isMarked;
    }

    // Check if already visited
    if (visited[curRow][curCol] == 1){
        return isMarked;
    }

    // Check if the current character in the board matches any child of the tree
    // Else, return
    int curChar = (int) tolower(p->board[curRow][curCol]);
    if (prefixTree->child[curChar] == NULL){
        return isMarked;
    }

    // Found a child, so mark visited and start recursively call on the compressed children group
    // Difference compared to part A is the grouping and compressed of each group
    struct prefixTree *nextTree = prefixTree->child[curChar];
    visited[curRow][curCol] = 1;

    // If the next tree contains a NULL character, we found a word, so mark it on the tree 
    // and increment wordCount if previously unmarked
    if (nextTree->child[(int) '\0'] != NULL && nextTree->child[(int) '\0']->visited == UNMARKED){
        nextTree->child[(int) '\0']->visited = MARKED;
        s->foundWordCount += 1;

        nextTree->visited = MARKED;
        isMarked = MARKED;
    }
    
    // First group those child with counting sort style
    int dimension = p->dimension;
    int curLabel = -1;
    convertToLabel(curRow, curCol, dimension, &curLabel);

    list_t* curChildren = adjacencyList[curLabel];
    list_t* childGroup[CHILD_COUNT];

    // Initialize those group
    for (int i=0; i < CHILD_COUNT; i++){
        childGroup[i] = NULL;
    }

    // Group the children by character
    groupSameCharacters(p, nextTree, childGroup, curChildren);

    // Iterate through the entire grouping
    int isChildMarked = UNMARKED;
    for (int i=0; i < CHILD_COUNT; i++){
        // Process each group: Compress into 1 primaryCell, traverse down that cell, and decompress the cell back
        isChildMarked = processCharacterGroup(childGroup[i], p, nextTree, adjacencyList, combineArray, s, visited);
        isMarked = isMarked || isChildMarked;
    }

    // Free the grouping
    for (int i=0; i < CHILD_COUNT; i++){
        if (childGroup[i] != NULL){
            freeList(childGroup[i]);
        }
    }

    // Backtrack: Unmark visited to allow a new path visiting the node
    visited[curRow][curCol] = 0;

    // Mark the tree from unmarked to marked
    // Don't update if node already marked as MARKED, since this node
    // might be required for some other path to found words, even if the current path
    // don't need it
    if (prefixTree->visited == UNMARKED && isMarked){
        prefixTree->visited = isMarked;
    }

    return isMarked;
}

/*
    Helper function: Set up the visited and found array similar to A, but use a different 
    traverse function and arguments
*/
void processingLogicProblemD(struct problem *p, struct solution *s, struct prefixTree *prefixTree){
    // Initialize visited and the found array
    int dimension = p->dimension;

    int **visited = malloc2dMatrix(dimension, dimension);
    for (int i=0; i < dimension; i++){
        for (int j=0; j < dimension; j++){
            visited[i][j] = 0;
        }
    }

    // Build the adjacencyList
    list_t** adjacencyList = buildAdjacencyList(p);
    // Create an array used to combine children of node efficiently (kinda like hashset)
    int* combineArray = (int *) malloc(sizeof(int) * dimension * dimension);
    assert(combineArray);

    // Do a similar grouping like inside the traverse function
    // in order to compress the initial starting node, but on the entire board,
    // thus speed up repeated, clustered initial node (if any)
    list_t* childGroup[CHILD_COUNT];

    // Initialize to null
    for (int i=0; i < CHILD_COUNT; i++){
        childGroup[i] = NULL;
    }

    // Group the board cells
    for (int i=0; i < dimension; i++){
        for (int j=0; j < dimension; j++){
            int curChar = (int) p->board[i][j];
            int curLabel = -1;
            convertToLabel(i, j, dimension, &curLabel);

            // Skip if character not in the tree
            if (prefixTree->child[tolower(curChar)] == NULL){
                continue;
            }

            // Else, add the node to its correct group
            // Initialize the list if empty
            if (childGroup[curChar] == NULL){
                childGroup[curChar] = createList();
            }

            prependNode(childGroup[curChar], curLabel);
        }
    }

    // Process each group
    for (int i=0; i < CHILD_COUNT; i++){
        processCharacterGroup(childGroup[i], p, prefixTree, adjacencyList, combineArray, s, visited);
    }

    // Free the grouping
    for (int i=0; i < CHILD_COUNT; i++){
        if (childGroup[i] != NULL){
            freeList(childGroup[i]);
        }
    }

    // Always mark the first root node as MARKED to start parsing
    prefixTree->visited = MARKED;

    // Convert the found array into the solution fomat required
    // using the same helper as A
    parseFoundWords(prefixTree, s, p);

    free2dMatrix(visited, dimension);

    // Free the adjacency matrix and the combine array
    freeAdjacencyList(adjacencyList, p);
    free(combineArray);
}




struct solution *solveProblemD(struct problem *p){
    struct solution *s = newSolution(p);
    s->foundWordCount = 0;
    /*
        First optimization: Filter out invalid word with repeated characters
    */
    struct prefixTree* prefixTree = newPrefixTree();

    // Only add word with non-repeated characters to the prefixTree
    int wordCount = p->wordCount;
    int validWordCount = 0;
    for (int i=0; i < wordCount; i++){
        char* curWord = p->words[i];
        if (checkRepeatedCharacters(curWord)){
            validWordCount++;
            addWordToTree(prefixTree, curWord, i);
        }
    }

    // Return early if nothing is valid
    if (validWordCount == 0){
        freePrefixTree(prefixTree);
        return s;
    }


    /*
        Second optimization: Compress the graph by fusing repeated characters during traversal
    */
    // Process the similar set up as A, traverse the board and the prefix tree with the compression algorithm
    processingLogicProblemD(p, s, prefixTree);

    // Free the tree
    freePrefixTree(prefixTree);

    return s;
}
