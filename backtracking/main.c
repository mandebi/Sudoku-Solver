#include <stdio.h>
#include <stdlib.h>
#include "backtracking.h"


int main(int argc, char **argv)
{
    FILE *fp=NULL; //file pointer
    char* filePath = argv[1];
    int decision;

    printf("    SUDOKU SOLVER! \n\n");

    resetSudokuMatrix(sudokuMatrix);//puts zeros in every cell of the sudoku array
    loadConfiguration(fp,filePath, sudokuMatrix);

    if(!checkViolation(sudokuMatrix))
        printf("\n\n  The initial SUDOKU set up is valid\n");
    else{
        printf("\n\n    The initial SUDOKU set up isn't valid\n");
        printSudokuMatrix(sudokuMatrix);
        return 0;
    }

    printf("\n    INITIAL SET UP! \n");
    printSudokuMatrix(sudokuMatrix);

    decision = solve(sudokuMatrix);

    if(!decision)
      printf("\n\nBAD NEWS: this Sudoku puzzle can't be solved !\n");
    else
      printf("\n\nGOOD NEWS: Sudoku puzzle solved in %d steps!\n", number_of_steps);

    printf("\n    FINAL SET UP!\n");
    printSudokuMatrix(sudokuMatrix);

    if(!checkViolation(sudokuMatrix))
        printf("\n\n  The final SUDOKU set up is valid\n");
    else
        printf("\n\n    The final SUDOKU set up isn't valid\n");

    return 0;
}
