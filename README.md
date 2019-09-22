<h1>SUDOKU-SOLVER in C</h1>



<h3>1- Introduction</h3>

This projects is about solving sudoky puzzles with two different approaches. The first one consists in iterating through
a space of possible solutions until finding one that is valid, while the second one uses propositional logic to define a set
of constraints that a SMT-solver will use to find a satisfiable solution.

<h3>2- Background</h3>
Sudoku is a logic game which objective is to fill a 9×9 (there ara other size for the game, but we only focus on 9×9 grids) cell grid with digits in such a way that each column, each row, and each of square of 3×3 cells contain all of the digits 1, 2, ... 9. The following image present an example of incomplete sudoku puzzle: (https://en.wikipedia.org/wiki/File:Sudoku_Puzzle_by_L2G-20050714_standardized_layout.svg)
<figure>
  <img src="images/example.png" height="42" width="42">
  <figcaption>Example of Sudoku Puzzle</figcaption>
</figure>

In order to provide a sudoku puzzle to our solver, we adopt the following representation:
<ul> 
  <li> Digits contained in the cells are separated by commas</li> 
  <li> Digit 0 represents empty cells</li> 
</ul>	
In both solving approaches (backtracking and SMT solver), we start by making sure that the initial configuration is correct (we don't have a digit appearing more than once on any row, column, and square. The adopted representation allows storing puzzles to solve into text file that are provided to the solvers. The following image depicts how the previous sudoku is represented in a text-file convenient way:
<figure>
  <img src="images/representation.png" height="42" width="42">
  <figcaption>Example of Puzzle formatted in a text file </figcaption>
</figure>



<h3>3- Backtracking Approach</h3>


<h3>Contact Information</h3>
<b>Author:</b> Joel Mandebi <br>
Ph.D. Student in Computer Engineering - University of Florida <br>
<b>Date:</b> Sept 22, 2019
