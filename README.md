# Parallel_Programming
Copy of course project Parallel Programming (IN2147) -- Optimazation and Parallelization on Gaussian Elimination

We implemented three methods (Openmp, MPI, SIMD) on forward and backward part of Gaussian Elimination.  
Please see the [report](/documentation/PPSS21_final_project.pdf) for results.


### OpenMP 
#### ForwardElimination

- Optimization of sequential code: There is some unnecessary repeated computation for "diagonal element" and "elimination factor" in the third "for" loop, so we move the computation of these parameters into the second "for" loop. In addition, because there is no access, in the following program, to those entries(the lower triangle of the matrix) that are supposed to be zero, we don't need to operate them anymore and just leave them as what they are.

- Parallization strategy: We implement the parallization in the second "for" loop because there is dependency between each step in the first "for" loop and the amount of computation of the third "for" loop is not large enough to implement parallization. Parallizing the second "for" loop would be a better choice. 

#### BackwardSubstitution

- We adjusted the order of the loops to avoid data race in parallel. Through row transmations to transformed the matrix into the identity one. However since the elements in the matrix won't be used any longer, we just need to correspondingly manipulate the `rhs` vector.  
In the file `ompge.cpp`, we also provide a parallel idea (a commented block of code) based on the method described in `serial.cpp`. Overall, there is little difference in the amount of computation and parallelism between the two methods, so the speedup is similar.
---

### MPI
#### ForwardElimination

We tried 2 ways to parallize ForwardElimination and we finally choose version 2, which might be slightly faster then version 1. The 2 parallization strategy would both be described in the following text and the codes of both strategy are also updated onto Gitlab. If you would like to try running version 1, please comment and uncomment some lines in the programm according to the guidance in the comments.

- Parallization strategy version 1: We implement the parallization in the second "for" loop. Each process will be resbonsible for a certain block of the rows. The blocks are equally distributed to all threads. There will be 3 conditions when running the algorithm. Suppose the variable `row` in the current round of the first "for" loop is `current_row`. The 1st condition is, `current_row+1` is above the block, which a certain process is resbonsible for. Then all rows in the block must do the elimination algorithm. The 2nd condition is, `current_row+1` is in the block. Then only the rows from `current_row+1` to the last row of the block should do the elimination algorithm. The 3rd condition is, `current_row+1` is under the block. Then no operation is needed in the block. At the end of the each round of the first "for" loop, the elements in the `current_row+1` of the matrix and rhs will be broadcasted to all processes from the process that is responsible for the block, which contains the `current_row+1`.
PS:The function ELiminate realizes the elimination algorithm for rows appointed to it.

- Parallization strategy version 2: Instead of letting each process be resbonsible for a continuous block, we now divide the rows into several blocks, whose size are the number of the processes, and each process is resbonsible for a row in such a block. There are two conditions when the loop variable `row` becomes larger during the loop: The 1st condition is, `row` is above the row that the current process is responsible for in the current block. The process should start its loop from the current block.The 2nd condition is, `row` is not above the row that the current process is responsible for in the current block. The process should start its loop from the next block. The other operation in this version is basically the same as version 1.

- Comparison between version 1 and version 2: In version 1, with the growing of the loop variable `row`, some processes might not be running because there would be no operation needed in the blocks that they are responsible for. And version 2 solve this problem. So, theoratically version 2 is a better parallization strategy.

#### BackwardSubstitution

- Parallization strategy: The strategy is similar to the ForwardElimination. Here we equally devide the columns into several blocks and each process would be responsible for one block. There are also 3 conditions. The 1st is, the `diag_idx` is on the left size of the block. Then all columns in the block should be operated. The 2nd is, the `diag_idx` is in the block. Then only the columns on the right side of the diag_idx should be operated. The 3rd is, the `diag_idx` is on the right side of the block. Then no operation is needed in this block. At the end of each round of the first "for" loop, the partial_sum of each blocks would be summed up and stored in rank 0 process as `total_sum`, which would be used to calculate the solution of the current row. After that, the solution would be broadcasted to all processes.

- Comparison between MPI parallized and unparallized BackwardSubstitution: Actually after several tests we found that the unparallized BackwardSubstitution is slightly faster than the parallized one. The reason might be that, the communication cost among the processes overwhelms the speedup from parallization. So, we remain the unparallized BackwardSubstitution in `mpige.cpp`. You could test it by uncommenting and commenting some lines according to the comments in the program.
---
### Hybrid

#### ForwardElimination

- The basic strategy is similar as the MPI ForwardElimination parallization strategy version 2. We just implement OMP to the second "for loop" in the function ForwardElimination.

#### BackwardSubstitution

- The basic strategy is similar as the MPI BackwardSubstitution parallization strateg. We found that implementing OMP to BackwardSubstitution will actually lead to a slower speed. So we don't implement OMP to it by default. If you would like to test it, please uncomment some lines according to the comments in the program.
---

### [SIMD](/bonus)

- In the code for Hybrid, we find a piece of code that can be applied with SIMD.
```
for (int column=row+1; column<columns; column++){
    element_idx = lower_rows*rows + column;
    matrix[element_idx] -= elimination_factor*matrix[row*rows+column];
}
```
Then we use AVX instruction set to form vector with 4 matrix elements as unit to carry out parallel operation.
