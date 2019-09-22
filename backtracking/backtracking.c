#include <stdio.h>
#include <stdlib.h>
#include "backtracking.h"



void resetSudokuMatrix(struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE]){
    int i, j;

    number_of_steps = 0;
    for (i = 0; i < NUM_LINE; i++)
      for (j = 0; j < NUM_LINE; j++){
           sudokuMatrix[i][j].digit = 0;
           sudokuMatrix[i][j].row = i;
           sudokuMatrix[i][j].col = j;
      }
}

void printSudokuMatrix(struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE]){
    int i, j;

    for (i = 0; i < NUM_LINE; i++){
        if((i%3==0)&&(i!=0))
           printf(" - - - - - - - - - - -\n");
      for (j = 0; j < NUM_LINE; j++){
           if((j%3==0)&&(j!=0))
              printf(" |");
         printf(" %d", sudokuMatrix[i][j].digit);
      }
      printf("\n");
    }
}

void loadConfiguration(FILE *fp, char* filePath,struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE]){
    int i, j, convertedValue, counter = 0;
    char readValue;

    fp = fopen(filePath, "r");

    for (i = 1; i <= NUM_LINE; i++){
        for (j = 1; j <= (NUM_LINE + NUM_LINE ); j++){
           readValue = fgetc(fp);
           if((j % 2) != 0){//we collect the values from the text file
             convertedValue = atoi(&readValue);
             sudokuMatrix[i-1][counter].digit = convertedValue;
             counter++;
           }
        }
       counter = 0;
    }
    fclose(fp);
}

int checkViolation(struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE]){
   int violation = 0,i,j,k,q;
   int begin_i, end_i, begin_j, end_j, counter_1 = 1;

   //We first check is a digit appears more than once on a column and row
   for (i = 0; i < NUM_LINE; i++){
    for (j = 0; j < NUM_LINE; j++){ //for the cell [i,j]
      if(sudokuMatrix[i][j].digit!=0){//we don't consider cells containing 0
        //we first look into its line (line i) to see if another cell holds the same value
        for(k=0;k<NUM_LINE;k++)
            if(k!=j)
                if(sudokuMatrix[i][k].digit == sudokuMatrix[i][j].digit){
                   printf("\nERROR: Conflict between cells - sudoku[%d][%d]=%d and sudoku[%d][%d]=%d\n",i+1,j+1, sudokuMatrix[i][j].digit, i+1,k+1, sudokuMatrix[i][k].digit);
                   violation = 1;
                   return  violation;
            }

        //we then look into its column (column j) to see if another cell holds the same value
        for(k=0;k<NUM_LINE;k++)
             if(k!=i)
                if(sudokuMatrix[k][j].digit == sudokuMatrix[i][j].digit){
                   printf("\nERROR: Conflict between cells - sudoku[%d][%d]=%d and sudoku[%d][%d]=%d\n",i+1,j+1, sudokuMatrix[i][j].digit, k+1,j+1, sudokuMatrix[k][j].digit);
                   violation = 1;
                   return  violation;
            }
      }
    }
   }

   begin_i = 0;
   end_i =2;
   begin_j = 0;
   end_j = 2;
   while(end_i<=8){
       if(counter_1==3){
          begin_i+=3;
          end_i+=3;
          begin_j=0;
          end_j=2;
          counter_1 = 1;
       }

       //Now we check each square of 9 digits to see if a value appears more than once
       for(i=begin_i;i<=end_i; i++){
        for(j=begin_j;j<=end_j; j++){
           if(sudokuMatrix[i][j].digit!=0){//we don't consider cells containing 0
              for(k=begin_i;k<=end_i; k++){
                for(q=begin_j;q<=end_j; q++){
                   if(k!=i && q!=j)
                      if(sudokuMatrix[k][q].digit == sudokuMatrix[i][j].digit){
                       printf("\nERROR: Conflict between cells - sudoku[%d][%d]=%d and sudoku[%d][%d]=%d\n",i+1,j+1, sudokuMatrix[i][j].digit, k+1,q+1, sudokuMatrix[k][q].digit);
                       violation = 1;
                       return  violation;
                      }
                }
              }
           }
        }
       }

       counter_1 ++;
       begin_j+=3;
       end_j+=3;
   }
    return violation;
}

struct CellContent findEmptyCell(struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE]){
  int i, j;
  struct CellContent cell;

  cell.col = -1;
  cell.digit = -1;
  cell.row = -1;

  for(i=0; i< NUM_LINE;i++)
    for(j=0;j<NUM_LINE;j++)
       if(sudokuMatrix[i][j].digit==0)
            return sudokuMatrix[i][j];//just to get the i,j coordinate of a cell that is empty

   return cell;
}


int isPlacementValid(struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE], int digit, int row, int col){

  int i,j;
  int square_i,square_j;

  //we first check the whole row
  for(j=0;j<NUM_LINE;j++)
    if(sudokuMatrix[row][j].digit == digit)
           return 0;

  //Then we check the whole column
  for(i=0;i<NUM_LINE;i++)
    if(sudokuMatrix[i][col].digit == digit)
           return 0;

  //Finaly, we check the square box when we want to insert
  square_i = row/3;
  square_j = col/3;

  for(i=0;i<NUM_SQUARE;i++)
    for(j=0;j<NUM_SQUARE;j++)
      if(sudokuMatrix[(square_i*3)+i][(square_j*3)+j].digit == digit)
           return 0;
   return 1;
}


//Function that returns 1 if we found the solution and 0 otherwise
int solve(struct CellContent sudokuMatrix [NUM_LINE][NUM_LINE]){

  struct CellContent empty_cell;
  int k;

  empty_cell = findEmptyCell( sudokuMatrix);

  if(empty_cell.digit==-1)//which means that there aren't empty cells no more: We then finished here!
      return 1;
  else{
       for(k=1; k<=NUM_LINE; k++){
          if(isPlacementValid(sudokuMatrix, k, empty_cell.row, empty_cell.col)){
              sudokuMatrix[empty_cell.row][empty_cell.col].digit = k;
              number_of_steps++;

              if(solve(sudokuMatrix))
                 return 1;

              sudokuMatrix[empty_cell.row][empty_cell.col].digit = 0;//Here we backtrack
              number_of_steps++;
          }
       }
       //if none of the 9 digit can be place on that cell, then the puzzle can't be solved
       return 0; //Reaching this means that there sudoku puzzle isn't solvable
  }

}





