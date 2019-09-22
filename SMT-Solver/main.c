#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#include<memory.h>
#include<setjmp.h>
#include<z3.h>
#include "utilities.h"


#define LOG_Z3_CALLS

#ifdef LOG_Z3_CALLS
#define LOG_MSG(msg) Z3_append_log(msg)
#else
#define LOG_MSG(msg) ((void)0)
#endif




/**
   \brief Create a variable using the given name and type.
*/
Z3_ast mk_var(Z3_context ctx, const char * name, Z3_sort ty)
{
    Z3_symbol   s  = Z3_mk_string_symbol(ctx, name);
    return Z3_mk_const(ctx, s, ty);
}

/**
   \brief Create a boolean variable using the given name.
*/
Z3_ast mk_bool_var(Z3_context ctx, const char * name)
{
    Z3_sort ty = Z3_mk_bool_sort(ctx);
    return mk_var(ctx, name, ty);
}


/**
   \brief Create a logical context.

   Enable model construction. Other configuration parameters can be passed in the cfg variable.

   Also enable tracing to stderr and register custom error handler.
*/
Z3_context mk_context_custom(Z3_config cfg, Z3_error_handler err)
{
    Z3_context ctx;

    Z3_set_param_value(cfg, "model", "true");
    ctx = Z3_mk_context(cfg);
    Z3_set_error_handler(ctx, err);

    return ctx;
}


/**
   \brief exit gracefully in case of error.
*/
void exitf(const char* message)
{
  fprintf(stderr,"BUG: %s.\n", message);
  exit(1);
}


/**
   \brief Simpler error handler.
 */
void error_handler(Z3_context c, Z3_error_code e)
{
    printf("Error code: %d\n", e);
    exitf("incorrect use of Z3");
}


/**
   \brief Display Z3 version in the standard output.
*/
void display_version()
{
    unsigned major, minor, build, revision;
    Z3_get_version(&major, &minor, &build, &revision);
    printf("Z3 %d.%d.%d.%d\n", major, minor, build, revision);
}

/**
   \brief Create a logical context.

   Enable model construction only.

   Also enable tracing to stderr and register standard error handler.
*/
Z3_context mk_context()
{
    Z3_config  cfg;
    Z3_context ctx;
    cfg = Z3_mk_config();
    ctx = mk_context_custom(cfg, error_handler);
    Z3_del_config(cfg);
    return ctx;
}



Z3_solver mk_solver(Z3_context ctx)
{
  Z3_solver s = Z3_mk_solver(ctx);
  Z3_solver_inc_ref(ctx, s);
  return s;
}

void del_solver(Z3_context ctx, Z3_solver s)
{
  Z3_solver_dec_ref(ctx, s);
}



/**
   \brief Check whether the logical context is satisfiable, and compare the result with the expected result.
   If the context is satisfiable, then display the model.
*/
void check(Z3_context ctx, Z3_solver s, Z3_lbool expected_result)
{
    Z3_model m      = 0;
    Z3_lbool result = Z3_solver_check(ctx, s);
    switch (result) {
    case Z3_L_FALSE:
        printf("\nThe puzzle in unsolvable\n");
        break;
    case Z3_L_UNDEF:
        printf("unknown\n");
        m = Z3_solver_get_model(ctx, s);
        if (m) Z3_model_inc_ref(ctx, m);
        printf("potential model:\n%s\n", Z3_model_to_string(ctx, m));
        break;
    case Z3_L_TRUE:
        /*m = Z3_solver_get_model(ctx, s);
        if (m) Z3_model_inc_ref(ctx, m);
        printf("sat\n%s\n", Z3_model_to_string(ctx, m));*/
        dumpModel(&ctx,s);
        publishZ3SudokuResult();
        
        break;
    }
    if (result != expected_result) {
        exitf("unexpected result");
    }
    if (m) Z3_model_dec_ref(ctx, m);
}


/**
  Function enforcing that in each 3x3 square, each digit must appear exactly once
*/
Z3_ast onlyOneLabelPerSquare(Z3_context* ctx, Z3_ast variables [NUM_LINE][NUM_LINE][NUM_LINE]){

  int i,j,k,l,u,v, counter_total_constraint = 0;
  int begin_i, end_i, begin_j, end_j, counter_1 = 1, square = 1,count=0;
  Z3_ast or_constraint, constraint;
  int cell_possible_entries = 9*9;
  Z3_ast or_variables [cell_possible_entries];
  Z3_ast temp1, temp2;
  Z3_ast temp_or[2];
  
  Z3_ast total_constraint [10000]; //contains the constraint for each cell that are conjuncted
 
  printf("* CONSTRAINT ENFORCEMENT: in each 3x3 square, each digit must appear: making it impossible to have the same digit more than once.\n");
      begin_i = 0;
      end_i =2;
      begin_j = 0;
      end_j = 2;
      while(end_i<=8){
       if(counter_1>3){
          begin_i+=3;
          end_i+=3;
          begin_j=0;
          end_j=2;
          counter_1 = 1;
       }
        if(square>9)
          break;

           for(k=0;k<NUM_LINE; k++){
            for(i=begin_i;i<=end_i; i++){
               for(j=begin_j;j<=end_j; j++){
                   or_variables[count++] =  variables[i][j][k];//Xijk: possible assignment into a cell(i,j)

                }//end for i
             }//end for j
             or_constraint =  Z3_mk_or (*ctx, count--, or_variables); //The content can be k = 1,2, ..., or 9: Xij1 V ...V Xij9
             total_constraint[counter_total_constraint] = or_constraint; 
             counter_total_constraint++;
              count=0;
         }//end for k

       counter_1 ++;
       begin_j+=3;
       end_j+=3;
       square++;


   }//end while
   constraint = Z3_mk_and (*ctx, counter_total_constraint,total_constraint );

  return constraint;
 }



/**
   Function creating the constraints enforcing that in each column, each digit must appear exactly once
*/
Z3_ast mustAppearOnceOnEachColumn(Z3_context* ctx, Z3_ast variables [NUM_LINE][NUM_LINE][NUM_LINE]){

  int i,j,k,l, counter_total_constraint = 0;
  Z3_ast or_constraint, constraint;
  int cell_possible_entries = 9;
  Z3_ast or_variables [cell_possible_entries];
  Z3_ast temp1, temp2;
  Z3_ast temp_or[2];
  
  Z3_ast total_constraint [10000]; //contains the constraint for each cell that are conjuncted
 
  printf("* CONSTRAINT ENFORCEMENT: in each column, each digit must appear exactly once.\n");
  
   //for each cell(i,j)
   for (k = 0; k < NUM_LINE; k++){
     for (j = 0; j < NUM_LINE; j++){
         //printf("\ni=%d - k=%d:\n", i,k); 
         for (i = 0; i < NUM_LINE; i++){
           or_variables[i] =  variables[i][j][k];//Xijk: possible assignment into a cell(i,j)

           /*char* string = Z3_ast_to_string (*ctx, variables[i][j][k]);
             printf("%s ", string); */

           temp1 = Z3_mk_not (*ctx, variables[i][j][k]); //not Xijk
           temp_or[0] = temp1; 
           
           for (l = 0; l < NUM_LINE; l++){
                if(l!=i){
                  temp2 = Z3_mk_not (*ctx, variables[l][j][k]);//not Xljk with l!=i
                  temp_or[1] = temp2;
                  total_constraint[counter_total_constraint] = Z3_mk_or(*ctx, 2, temp_or);
                  counter_total_constraint++;
                }
             }   

         }//end of for i
        or_constraint =  Z3_mk_or (*ctx, cell_possible_entries, or_variables); //The content can be i = 1,2, ..., or 9: X1jk V ...V X9jk
        /*char* string1 = Z3_ast_to_string (*ctx, or_constraint);
             printf("\nor_constraint = %s \n\n", string1);*/

        total_constraint[counter_total_constraint] = or_constraint;
        counter_total_constraint++;
     }//end of for j

   } //end of for k
     
   constraint = Z3_mk_and (*ctx, counter_total_constraint,total_constraint );

   /*char* string2 = Z3_ast_to_string (*ctx, constraint);
             printf("\nTOTAL CONSTRAINT = %s \n\n", string2);*/

  return constraint;
 }




/**
   Function creating the constraints enforcing that in each row, each digit must appear exactly once
*/
Z3_ast mustAppearOnceOnEachRow(Z3_context* ctx, Z3_ast variables [NUM_LINE][NUM_LINE][NUM_LINE]){

  int i,j,k,l, counter_total_constraint = 0;
  Z3_ast or_constraint, constraint;
  int cell_possible_entries = 9;
  Z3_ast or_variables [cell_possible_entries];
  Z3_ast temp1, temp2;
  Z3_ast temp_or[2];
  
  Z3_ast total_constraint [10000]; //contains the constraint for each cell that are conjuncted
 
  printf("* CONSTRAINT ENFORCEMENT: in each row, each digit must appear exactly once.\n");
  
   //for each cell(i,j)
   for (k = 0; k < NUM_LINE; k++){
     for (i = 0; i < NUM_LINE; i++){
         //printf("\ni=%d - k=%d:\n", i,k); 
         for (j = 0; j < NUM_LINE; j++){
           or_variables[j] =  variables[i][j][k];//Xijk: possible assignment into a cell(i,j)

           temp1 = Z3_mk_not (*ctx, variables[i][j][k]); //not Xijk
           temp_or[0] = temp1; 
           
           for (l = 0; l < NUM_LINE; l++){
                if(l!=j){
                  temp2 = Z3_mk_not (*ctx, variables[i][l][k]);//not Xilk with l!=j
                  temp_or[1] = temp2;
                  total_constraint[counter_total_constraint] = Z3_mk_or(*ctx, 2, temp_or);
                  counter_total_constraint++;
                }
             }   

         }//end of for j
        or_constraint =  Z3_mk_or (*ctx, cell_possible_entries, or_variables); //The content can be j = 1,2, ..., or 9: Xi1k V ...V Xi9k
        total_constraint[counter_total_constraint] = or_constraint;
        counter_total_constraint++;
     }//end of for i

   } //end of for k
   constraint = Z3_mk_and (*ctx, counter_total_constraint,total_constraint );
  return constraint;
 }




/**
 For each cell(i,j):
   1- The content can be k = 1,2, ..., or 9
   2- If the cell contains k = 1,2, ..., or 9, we can't add a value equal to k again
   3- If the cell contains k = 1,2, ..., or 9, we can't add a value equal l = 1,2, ..., or 9 with k != l
*/

 Z3_ast onlyOneLabelPerCell(Z3_context* ctx, Z3_ast variables [NUM_LINE][NUM_LINE][NUM_LINE]){

  int i,j,k,l, counter_total_constraint = 0;
  Z3_ast or_constraint, constraint;
  int cell_possible_entries = 9;
  Z3_ast or_variables [cell_possible_entries];
  Z3_ast temp1, temp2;
  Z3_ast temp_or[2];
  
  Z3_ast total_constraint [10000]; //contains the constraint for each cell that are conjuncted
 
  printf("* CONSTRAINT ENFORCEMENT: a cell is assigned only one value.\n");

   //for each cell(i,j)
   for (i = 0; i < NUM_LINE; i++){
      for (j = 0; j < NUM_LINE; j++){
         for (k = 0; k < NUM_LINE; k++){
             or_variables[k] =  variables[i][j][k];//Xijk: possible assignment into a cell(i,j)

             temp1 = Z3_mk_not (*ctx, variables[i][j][k]); //not Xijk
             temp_or[0] = temp1;   
             for (l = 0; l < NUM_LINE; l++){
                if(l!=k){
                  temp2 = Z3_mk_not (*ctx, variables[i][j][l]);//not Xijl with l!=k
                  temp_or[1] = temp2;
                  total_constraint[counter_total_constraint] = Z3_mk_or(*ctx, 2, temp_or);
                  counter_total_constraint++;
                }
             }   
      }//end of for k
      or_constraint =  Z3_mk_or (*ctx, cell_possible_entries, or_variables); //The content can be k = 1,2, ..., or 9: Xij1 V ...V Xij9
      total_constraint[counter_total_constraint] = or_constraint;
      counter_total_constraint++;
    }//end of for j
  }//end of for i
   
   constraint = Z3_mk_and (*ctx, counter_total_constraint,total_constraint );

  return constraint;
 }



/**
  Main Function
*/
int main(int argc, char **argv) {
#ifdef LOG_Z3_CALLS
    Z3_open_log("z3.log");
#endif
    display_version();


    FILE *fp=NULL; //file pointer
    char* filePath = argv[1];
    int i,j,k, constraint_counter = 0;
    Z3_ast variables [NUM_LINE][NUM_LINE][NUM_LINE], true_node;
    Z3_ast constraints[10000];


    printf("   +------------------------+\n");
    printf("   | SUDOKU SOLVER WITH Z3! |\n");
    printf("   +------------------------+\n");

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

    encodeSudoKuMatrix();

    //Here we can start building our Z3 Model
    Z3_context ctx = mk_context();
    Z3_solver  s = mk_solver(ctx);

    true_node = Z3_mk_true (ctx);

    //We dynamically create our 9*9*9=729 boolean variables
    for (i = 0; i < NUM_LINE; i++)
      for (j = 0; j < NUM_LINE; j++)
          for (k = 0; k < NUM_LINE; k++){
              variables[i][j][k] = mk_bool_var(ctx, variable_list[i][j][k].var_name); 

              if(variable_list[i][j][k].truth_value==1){ // the cell is already assigned a content
                constraints[constraint_counter] = Z3_mk_eq (ctx, variables[i][j][k], true_node);
                constraint_counter ++;  
              }
    }

   
   //Constraint1: Only one digit per cell 
   printf("BUILDING CONSTRAINTS ... \n"); 
  
    
    constraints[constraint_counter] = onlyOneLabelPerCell(&ctx,variables);
    constraint_counter ++;
   
    constraints[constraint_counter] = mustAppearOnceOnEachRow(&ctx,variables);
    constraint_counter ++;
    
    constraints[constraint_counter] = mustAppearOnceOnEachColumn(&ctx,variables);
    constraint_counter ++;

    constraints[constraint_counter] = onlyOneLabelPerSquare(&ctx,variables);
    constraint_counter ++;

   Z3_ast system = Z3_mk_and (ctx, constraint_counter,constraints );
   
   printf("ASSERTING Z3 SOLVER ... \n");  
    
    Z3_solver_assert(ctx, s, system); 
    check(ctx, s, Z3_L_TRUE);
  
    del_solver(ctx, s);
    Z3_del_context(ctx);

    printf("\n    FINAL SET UP! \n");
    printSudokuMatrix(sudokuMatrix);
    if(!checkViolation(sudokuMatrix))
        printf("\n  The final SUDOKU set up is valid\n");
    else{
        printf("\n    The final SUDOKU set up isn't valid\n");
        return 0;
    }


    return 0;

}