# Code Changes
## ForwardSubstitution

- Optimization of sequential code: There is some unnecessary repeated computation for "diagonal element" and "elimination factor" in the third "for" loop, so we move the computation of these parameters into the second "for" loop. In addition, because there is no access, in the following program, to those entries(the lower triangle of the matrix) that are supposed to be zero, we don't need to operate them anymore and just leave them as what they are.

- Parallization strategy: We implemented the parallization in the second "for" loop because there is dependency between each step in the first "for" loop and the amount of computation of the third "for" loop is not large enough to implement parallization. Parallizing the second "for" loop would be a better choice. 
## BackwardSubstitution

- We adjusted the order of the loops to avoid data race in parallel. Through row transmations to transformed the matrix into the identity one. However since the elements in the matrix won't be used any longer, we just need to correspondingly manipulate the `rhs` vector.  
In the file `ompge.cpp`, we also provide a parallel idea (a commented block of code) based on the method described in `serial.cpp`. Overall, there is little difference in the amount of computation and parallelism between the two methods, so the speedup is similar.
