// Sudoku puzzle verifier and solver

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* structure for passing data to threads */
  typedef struct {
      // row num
      int row;
      // column num
      int column;
      // grid representing the puzzle
      int** puzzle;
      // length/width of puzzle
      int size;
      // array representing validity of indexes
      int* validity;

  } Parameters;

void checkRow(Parameters* params);
void checkCol(Parameters* params);

// takes puzzle size and grid[][] representing sudoku puzzle
// and tow booleans to be assigned: complete and valid.
// row-0 and column-0 is ignored for convenience, so a 9x9 puzzle
// has grid[1][1] as the top-left element and grid[9]9] as bottom right
// A puzzle is complete if it can be completed with no 0s in it
// If complete, a puzzle is valid if all rows/columns/boxes have numbers from 1
// to psize For incomplete puzzles, we cannot say anything about validity
void checkPuzzle(int psize, int **grid, bool *complete, bool *valid) {

  // integer array that determines the validity of each row in the puzzle
  // 0: INVALID, 1: VALID, 2: INCOMPLETE
  // add 1 so that we can ignore index zero and have row number correspond with validity index
  int* rowValidity = malloc((psize + 1) * sizeof(*rowValidity));

  // integer array that determines the validity of each column in the puzzle
  // 0: INVALID, 1: VALID, 2: INCOMPLETE
  // add 1 so that we can ignore index zero and have column number correspond with validity index
  int* colValidity = malloc((psize + 1) * sizeof(*colValidity));

  // integer array that determines the validity of each box in the puzzle
  // 0: INVALID, 1: VALID, 2: INCOMPLETE
  // add 1 so that we can ignore index zero and have box number correspond with validity index
  int* boxValidity = malloc((psize + 1) * sizeof(*boxValidity));

  // initialize all values in solution arrays to -1
  for (int i = 1; i <= psize; i++) {
    rowValidity[i] = -1;
    colValidity[i] = -1;
    boxValidity[i] = -1;
  }

  // testing row checker
  Parameters* params = (Parameters*) malloc(sizeof(Parameters));
  params->row = 1;
  params->column = 2;
  params->puzzle = grid;
  params->size = psize;
  params->validity = colValidity; 
  checkCol(params);

  // values being checked
  printf("VALUES BEING CHECKED:\n");
  for (int i = 1; i <= psize; i++) {
    printf("index %d: %d\n", i, grid[i][params->column]);
  }

  // check rowValidity for correct values
  printf("COLUMN VALIDITY TEST:\n");
  for (int i = 1; i <= psize; i++) {
    printf("index %d: %d\n", i, colValidity[i]);
  }

  *valid = false;
  *complete = false;
}

// takes filename and pointer to grid[][]
// returns size of Sudoku puzzle and fills grid
int readSudokuPuzzle(char *filename, int ***grid) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Could not open file %s\n", filename);
    exit(EXIT_FAILURE);
  }
  int psize;
  fscanf(fp, "%d", &psize);
  int **agrid = (int **)malloc((psize + 1) * sizeof(int *));
  for (int row = 1; row <= psize; row++) {
    agrid[row] = (int *)malloc((psize + 1) * sizeof(int));
    for (int col = 1; col <= psize; col++) {
      fscanf(fp, "%d", &agrid[row][col]);
    }
  }
  fclose(fp);
  *grid = agrid;
  return psize;
}

// takes puzzle size and grid[][]
// prints the puzzle
void printSudokuPuzzle(int psize, int **grid) {
  printf("%d\n", psize);
  for (int row = 1; row <= psize; row++) {
    for (int col = 1; col <= psize; col++) {
      printf("%d ", grid[row][col]);
    }
    printf("\n");
  }
  printf("\n");
}

// takes puzzle size and grid[][]
// frees the memory allocated
void deleteSudokuPuzzle(int psize, int **grid) {
  for (int row = 1; row <= psize; row++) {
    free(grid[row]);
  }
  free(grid);
}

// determines whether a certain row in the puzzle is valid
// takes in the row/ col information to check, the grid, the row/column size, 
// and the array containing the validity values for rows
// for rowValidity -> 0: INVALID, 1: VALID, 2: INCOMPLETE
void checkRow(Parameters* params) {

  // contains indexes for each value in the row
  // 0 means 'not found', 1 means 'found'
  int foundVals[params->size + 1];

  // initialize all values to zero (not found)
  for (int i = 1; i <= params->size; i++) {
    foundVals[i] = 0;
  }

  // debugging test
  for (int i = 1; i <= params->size; i++) {
    printf("checking index: (%d, %d)\n", params->row, i);
    printf("value is: %d\n", params->puzzle[params->row][i]);
    foundVals[params->puzzle[params->row][i]] = 1;
  }

  // check foundVals
  printf("TEST FOUNDVALS:\n");
  for (int i = 1; i <= params->size; i++) {
    printf("index %d: %d\n", i, foundVals[i]);
  }

  // check if there are any zeros in 'foundVals', which would mean that the row is not valid
  bool valid = true;
  for (int i = 1; i <= params->size; i++) {
    if (foundVals[i] == 0) {
      valid = false;
      break;
    }
  }
  if (!valid) {
    params->validity[params->row] = 0;
  }
  else {
    params->validity[params->row] = 1;
  }

}

// determines whether a certain column in the puzzle is valid
// takes in the row/col information to check, the grid, the row/column size, 
// and the array containing the validity values for rows
// for rowValidity -> 0: INVALID, 1: VALID, 2: INCOMPLETE
void checkCol(Parameters* params) {

  // contains indexes for each value in the row
  // 0 means 'not found', 1 means 'found'
  int foundVals[params->size + 1];

  // initialize all values to zero (not found)
  for (int i = 1; i <= params->size; i++) {
    foundVals[i] = 0;
  }

  // debugging test
  for (int i = 1; i <= params->size; i++) {
    printf("checking index: (%d, %d)\n", params->column, i);
    printf("value is: %d\n", params->puzzle[i][params->column]);
    foundVals[params->puzzle[i][params->column]] = 1;
  }

  // check foundVals
  printf("TEST FOUNDVALS:\n");
  for (int i = 1; i <= params->size; i++) {
    printf("index %d: %d\n", i, foundVals[i]);
  }

  // check if there are any zeros in 'foundVals', which would mean that the row is not valid
  bool valid = true;
  for (int i = 1; i <= params->size; i++) {
    if (foundVals[i] == 0) {
      valid = false;
      break;
    }
  }
  if (!valid) {
    params->validity[params->column] = 0;
  }
  else {
    params->validity[params->column] = 1;
  }

}

// expects file name of the puzzle as argument in command line
int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: ./sudoku puzzle.txt\n");
    return EXIT_FAILURE;
  }
  // grid is a 2D array
  int **grid = NULL;
  // find grid size and fill grid
  int sudokuSize = readSudokuPuzzle(argv[1], &grid);
  bool valid = false;
  bool complete = false;
  checkPuzzle(sudokuSize, grid, &complete, &valid);
  printf("Complete puzzle? ");
  printf(complete ? "true\n" : "false\n");
  if (complete) {
    printf("Valid puzzle? ");
    printf(valid ? "true\n" : "false\n");
  }
  printSudokuPuzzle(sudokuSize, grid);
  deleteSudokuPuzzle(sudokuSize, grid);
  return EXIT_SUCCESS;
}

