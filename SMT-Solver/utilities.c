#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilities.h"
#include<z3.h>


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


void dumpModel(Z3_context* ctx, Z3_solver s){
    FILE *fp;
    Z3_model m = 0;
    
    
    fp = fopen("model.txt", "w");

    if(fp == NULL)
     {
      printf("Error! The program needs to be able to create a file on the project's folder. Please grant file creation rights\n");   
      exit(1);             
     }


    m = Z3_solver_get_model(*ctx, s);
        if (m) Z3_model_inc_ref(*ctx, m);
        char* readValue  = Z3_model_to_string(*ctx, m); 
        fprintf(fp,"%s", readValue);

    fclose(fp);
}



void publishZ3SudokuResult(){
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int i,j,k;
  char c1[2],c2[2],c3[2];

  fp = fopen("model.txt", "r");
  while ((read = getline(&line, &len, fp)) != -1) {
    if (strstr(line, "true") != NULL) {//if Xijk is true then Cell(i,j) = k
      c1[0] = line[1];
      c1[1] = '\0'; 

      c2[0] = line[2];
      c2[1] = '\0';

      c3[0] = line[3];
      c3[1] = '\0';

      i = atoi(c1);
      j = atoi(c2);
      k = atoi(c3);
      
      sudokuMatrix [i][j].digit = k;
    }

  }
 
  free(line);
  


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

int encodeSudoKuMatrix(){
  
  int i, j, k, counter=0;
 
  printf("\n");


  for (i = 0; i < NUM_LINE; i++)
      for (j = 0; j < NUM_LINE; j++)
          for (k = 0; k < NUM_LINE; k++){
              variable_list[i][j][k].i = i;
              variable_list[i][j][k].j = j;
              variable_list[i][j][k].k = k;
              
              variable_list[i][j][k].var_name[0] = 'x';
              variable_list[i][j][k].var_name[1] = i+'0';
              variable_list[i][j][k].var_name[2] = j+'0';
              variable_list[i][j][k].var_name[3] = (k+1)+'0';
              variable_list[i][j][k].var_name[4] = '\0';
              
              if(sudokuMatrix[i][j].digit==k+1)
                 variable_list[i][j][k].truth_value = 1;
              else 
                 variable_list[i][j][k].truth_value = -1; 

              counter++;
          }


}


