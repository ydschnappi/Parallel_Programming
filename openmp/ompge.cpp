//
// Created by shubham on 14.05.21.
//

#include <cassert>
#include <chrono>
#include <omp.h>
#include "ompge.h"
#define CHUNKSIZE 4
namespace OMP{


    void ForwardElimination(double *matrix, double *rhs, int rows, int columns){
        
        for(int row = 0; row < rows; row++){
            #pragma omp parallel for
            for (int lower_rows = row+1; lower_rows<rows; lower_rows++){
                int below_diag_idx = lower_rows*rows + row;
                // Extract diagonal element
                int diag_idx = row*rows + row;
                double diag_elem = matrix[diag_idx];
                assert(diag_elem != 0);
                // Compute the factor
                double elimination_factor = matrix[below_diag_idx]/diag_elem;
                int element_idx;
                for (int column = row+1; column<columns; column++){
                    // Set the column index of the entry to be operated
                    element_idx = lower_rows*rows + column;
                    // Subtract the row
                    matrix[element_idx] -= elimination_factor*matrix[row*rows+column];
                }
                rhs[lower_rows] -= elimination_factor*rhs[row];
                // Set below diagonal elements to 0, but in the following program
                // we don't need to access them, so no more operation needed
                // matrix[below_diag_idx] = 0.;
            }
        }
    }


    void BackwardSubstitution(double *matrix, double *rhs, double*solution, int rows, int columns){
        //Calculate x_(rows-1)
        solution[rows-1] = rhs[rows-1]/matrix[rows*columns-1];
        //matrix[rows*columns-1] = 1;
        //rhs[rows-1] = solution[rows-1];
        int column = columns-1; 
        while(column){
            //Calculate for x_(column-1)
            #pragma omp parallel for
            for(int row = 0;row < column;row++){
                int element_idx = row*columns + column;
                rhs[row] -= matrix[element_idx]*solution[column];
                //matrix[element_idx] = 0;
            }
            column--;
            int diag_idx = column*columns + column;
            solution[column] = rhs[column] / matrix[diag_idx];
            //set diagonal elements to 1;however, since this element won't be used no longer,this step can be ommited
            //matrix[diag_idx]=1;
        }
    }
}


//This is the another way to parallel BackwardSubstitution. It just made change to the loop order so no big difference on speedup

// void BackwardSubstitution(double *matrix, double *rhs, double*solution, int rows, int columns){
//     {
//     for(int row=rows-1; row>=0; row--){
//         solution[row] = rhs[row];
//         int diag_idx = row*rows + row;
//         double sum2substract = 0;
//         #pragma omp parallel for reduction(+:sum2substract) 
//         for (int column=row+1; column<columns; column++){
//             {
//             int element_idx = row*rows + column;
//             sum2substract += matrix[element_idx]*solution[column];
//             }
//         }
//         solution[row] = (solution[row] - sum2substract)/matrix[diag_idx];
//     }
//     }
// }
// }


void OMP::Solve(double *matrix, double *rhs, double *solution,
                   int rows, int columns){
    OMP::ForwardElimination(matrix, rhs, rows, columns);
    OMP::BackwardSubstitution(matrix, rhs, solution, rows, columns);
}

