# COMP20007 Assignment 2 (Mark: 19.5/20)
## Acknowledgement
All questions and skeleton code is from COMP20007 Sem 1 2024 (UniMelb), as specified in each file.

The main implementation is implemented by me, located in the file problem.c mainly (although additional data structures might be implemented as well).

## Note:
For task 1 part E written report, there's a typo where best case analysis is denoted as Theta, while it should be
Omega. 

## Task 1: Dynamic Time Warping (DP)

The task is included in the pdf, mainly utilize DP to check the "similarity" between 2 stretched sequence in O(mn), with m and n being the length of the 2 compared sequences. 

The last task requires an additional dimension to take care of the path length.

## Task 2: Boggle Search (Prefix Tree and Backtrack)

An interesting problem, as detailed in the pdf and the analysis in written folder.

Main goal was to find possible word that can be found from a board, making sure that the word is one of the given words in the dictionary (basically boggle on n x n board).

Backtracking is used to search the board, while a prefix trie is used to efficiently check if the the current sequence is still valid (the word is from the dictionary).

The last task asked for optimisation to speed up runtime with repeated characters. This requires a "compression" algorithm while backtracking on the board (along with a bit of initial filtering). Details about the improvement (and its time complexity) is included in the written folder.

### Potential Improvements:
The current implementation is quite efficient, making use of adjacency list for a graph representation of the board (since the graph is sparse). All test cases included in this repo takes less than 100 ms to run with the current implementation.

Still, the compression is requiring several memory allocation, traded off for simplicity. The linked list, however, can be modified to avoid this:

1. Set up a second path of pointers utilizing for grouping/ compressing
2. Include in the linkedlist a metadata to include which path of pointers is currently used
3. For compressing, we can use the second path to link allocated nodes
4. For decompressing, simply switch back to the first path (containing the original link)
5. Might need to update pointers during step 4

Same asymptotic time complexity, but since only the link is changed, with no memory allocation, should be practically faster.

## Run the program
1. Run "make problemXX"
2. Go to the "problemXX.c" file for instructions to run