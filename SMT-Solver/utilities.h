#ifndef UTILITIES_H
#define UTILITIES_H

#define NUM_LINE 9
#define DEBUG 0

#include<z3.h>

int number_of_steps;

struct CellContent {
  int digit;
  int row;
  int col;
};

struct BooleanCell{
  int i;
  int j;
  int k;
  char var_name [5];
  int truth_value; //-1 if unknow, 1 if true, and 0 if false

};

//struct BooleanCell variable_list [NUM_VARIABLES];
struct BooleanCell variable_list [NUM_LINE][NUM_LINE][NUM_LINE];
struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE];


/**Functions**/
void resetSudokuMatrix(struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE]);// puts zeros in every cell of the sudoku array
void printSudokuMatrix(struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE]);//print the content of the current sudoku array
void loadConfiguration(FILE *fp, char* filePath ,struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE]);//load a text file into the sudoku array variable
int checkViolation(struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE]);//checks if the current sudoku array respects the rules


int encodeSudoKuMatrix();
void dumpModel(Z3_context* ctx, Z3_solver s);
void publishZ3SudokuResult();

#endif // BACKTRACKING_H_INCLUDED
