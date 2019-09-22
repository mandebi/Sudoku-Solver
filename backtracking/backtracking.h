#ifndef BACKTRACKING_H
#define BACKTRACKING_H

#define NUM_LINE 9
#define NUM_SQUARE 3
#define DEBUG 0

int number_of_steps;

struct CellContent {
  int digit;
  int row;
  int col;
};

struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE];


/**Functions**/
void resetSudokuMatrix(struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE]);// puts zeros in every cell of the sudoku array
void printSudokuMatrix(struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE]);//print the content of the current sudoku array
void loadConfiguration(FILE *fp, char* filePath ,struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE]);//load a text file into the sudoku array variable
int checkViolation(struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE]);//checks if the current sudoku array respects the rules
struct CellContent findEmptyCell(struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE]);

int isPlacementValid(struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE], int digit, int row, int col);//checks if a digit can go into a cell considering the row, column and square box

int solve(struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE]);

#endif // BACKTRACKING_H_INCLUDED
