#include "mpige.h"
#include <chrono>
#include <cassert>
#include <mpi.h>
#include "mpige.h"
#define OMPI_SKIP_MPICXX 1

namespace MPI{
    // ForwardElimination v2. See Readme.
    void ForwardElimination_v2(double *matrix, double *rhs, int rows, int columns, int rank, int size){
        for(int row = 0; row < rows-1; row++){
            int block_idx = row / size;
            int l_rows_start;
            // 1st condition, "row" is above the row that the current process is responsible for
            // in the current "size" size block. The process should start its loop from the current "size" size block.
            if((row%size)<rank){
                l_rows_start = block_idx*size + rank;
            }
            // 2nd condition, "row" is not above the row that the current process is responsible for
            // in the current "size" size block. The process should start its loop from the next "size" size block.
            else{
                l_rows_start = (block_idx+1)*size + rank;
            }
            // Extract Diagonal element
            int diag_idx = row*rows + row;
            double diag_elem = matrix[diag_idx];
            assert(diag_elem!=0);
            for (int lower_rows=l_rows_start; lower_rows<rows; lower_rows+=size){
                int below_diag_idx = lower_rows*rows + row;
                // Compute the factor
                double elimination_factor = matrix[below_diag_idx]/diag_elem;
                int element_idx;
                for (int column=row+1; column<columns; column++){
                    // Set the column index of the entry to be operated
                    element_idx = lower_rows*rows + column;
                    // Subtract the row
                    matrix[element_idx] -= elimination_factor*matrix[row*rows+column];
                }
                rhs[lower_rows] -= elimination_factor*rhs[row];
            } 
            // after the elimination, we need to broadcast the newest row+1 to all threads,
            // so as to get ready for the next round and  the process with rank 0 will finally 
            // keep the result of ForwardSubstitution
            int r = (row+1)%size;
            MPI_Bcast(matrix+(row+1)*rows, columns, MPI_DOUBLE, r, MPI_COMM_WORLD);
            MPI_Bcast(rhs+row+1, 1, MPI_DOUBLE, r, MPI_COMM_WORLD);
        }
    }

    // --------------------------------------------------------------------------------------------
    // The function BackwardSubstitution is a parallized version. If you would like to run a 
    // unparallized version of BackwardSubstitution, which is slightly faster then the parallized
    // one, you could uncomment the function BackwardSubstitution_rank0.
    // --------------------------------------------------------------------------------------------

    void BackwardSubstitution(double *matrix, double *rhs, double*solution,
                int rows, int columns, int rank, int size){
        // Set the size of the block, each thread would be responsible for block-size columns
        
        int block = columns / size;
        
        for(int row=rows-1; row>=0; row--){
            // Used to store the sum of parameters that ready to be extracted from rhs in each block
            double partial_sum = 0.0;
            // Used to sum up partial_sum
            double total_sum = 0.0;
            int diag_idx = row*rows + row;
            // 1st condition,row is smaller then block*rank
            // All columns in this block should be operated
            if(row < block*rank){
                for (int column=block*rank; column<block*(rank+1); column++){
                    int element_idx = row*rows + column;
                    partial_sum += matrix[element_idx]*solution[column];
                }
            }
            // 2nd condition, number of row is in the block that the current thread is responsible for
            // Only columns in thes block that are larger then row should be operated
            else if(row>= block*rank && row<block*(rank+1)){
                for (int column=row+1; column<block*(rank+1); column++){
                    int element_idx = row*rows + column;
                    partial_sum += matrix[element_idx]*solution[column];
                    // solution[row] -= matrix[element_idx]*solution[column];
                }
            }
            // Sum up the partial_sum and store it in tota_sum in rank 0
            MPI_Reduce(&partial_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
            // Calculate the solution of the current row in rank 0
            if(rank == 0){
                solution[row] = (rhs[row] - total_sum) / matrix[diag_idx];
            }
            // Broadcast the solution to all other threads because the solution might be used in the next round
            MPI_Bcast(&solution[row], 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        }
    }

    // -------------------------------------------------------------------------------------------------------------------------
    // ForwardElimination Version 1, see Readme. If you would like to run Version 1, please uncomment the following 2 functions
    // Eliminate_v1 and ForwardElimination_v1

    // -------------------------------------------------------------------------------------------------------------------------
    // The function Eliminate will compute the elements in matrix from row l_row_start to the 
    // previous row of for_limit as the second "for loop" of  algorithm  Forward Substitution 
    // -------------------------------------------------------------------------------------------------------------------------
    // void Eliminate(double *matrix, double *rhs, int row, int rows, int columns, int l_rows_start, int for_limit){
    //     // Extract Diagonal element
    //     int diag_idx = row*rows + row;
    //     double diag_elem = matrix[diag_idx];
    //     assert(diag_elem!=0);
    //     for (int lower_rows=l_rows_start; lower_rows<for_limit; lower_rows++){
    //         int below_diag_idx = lower_rows*rows + row;
    //         // Compute the factor
    //         double elimination_factor = matrix[below_diag_idx]/diag_elem;
    //         int element_idx;
    //         for (int column=row+1; column<columns; column++){
    //              // Set the column index of the entry to be operated
    //              element_idx = lower_rows*rows + column;
    //             // Subtract the row
    //             matrix[element_idx] -= elimination_factor*matrix[row*rows+column];
    //         }
    //         rhs[lower_rows] -= elimination_factor*rhs[row];
    //     }   
    // }
    // 
    // void ForwardElimination_v1(double *matrix, double *rhs, int rows, int columns, int rank, int size){
    //     // Set the size of the block, each thread would be responsible for block-size rows
    //     int block = rows / size;
    //     for(int row = 0; row < rows-1; row++){
    //         // 1st condition, row+1 is above the block that the current thread is responsible for
    //         // All rows in this block will do elimination
    //         if(row < rank*block){
    //             // Define the start row of Elimination
    //             int l_rows_start = rank*block;
    //             // Define the end of Elimination
    //             int for_limit = (rank+1)*block;
    //             Eliminate(matrix, rhs, row, rows, columns, l_rows_start, for_limit);
    //         }
    //         // 2nd condition, row+1 is in the block that the current thread is responsible for
    //         // only the rows larger than row should do elimination
    //         else if(row>=rank*block && row<(rank+1)*block){
    //             // Define the start row of Elimination
    //             int l_rows_start = row+1;
    //             // Define the end of Elimination
    //             int for_limit = (rank+1)*block;
    //             Eliminate(matrix, rhs, row, rows, columns, l_rows_start, for_limit);
    //         }
    //         // --------------------------------------------------------------------------------
    //         // 3rd condition, row+1 is under the block that current thread is responsible for
    //         // no operation needed
    //         // --------------------------------------------------------------------------------

    //         // after the elimination, we need to broadcast the newest row+1 to all threads,
    //         // so as to get ready for the next round and  the thread with rank 0 will finally 
    //         // keep the result of ForwardSubstitution
    //         int r = (row+1) / block;
    //         MPI_Bcast(matrix+(row+1)*rows, columns, MPI_DOUBLE, r, MPI_COMM_WORLD);
    //         MPI_Bcast(rhs+row+1, 1, MPI_DOUBLE, r, MPI_COMM_WORLD);
    //     }
    // }
    

    //-----------------------------------------------------------------------------------------------------------
    // The function BackwardSubstitution_rank0 is for the Backward Substitution without parallization.
    // With several tests, it shows that Backward Subtsitution without parallization is slightly faster than  
    // that is parallized. 
    // Uncommont this function if you would like to test the programm with unparallized Backward Substitution
    // ----------------------------------------------------------------------------------------------------------
    
    // void BackwardSubstitution_rank0(double *matrix, double *rhs, double*solution, int rows, int columns){
    //     for(int row=rows-1; row>=0; row--){
    //         solution[row] = rhs[row];
    //         int diag_idx = row*rows + row;
    //         for (int column=row+1; column<columns; column++){
    //             int element_idx = row*rows + column;
    //             solution[row] -= matrix[element_idx]*solution[column];
    //         }
    //         solution[row] /= matrix[diag_idx];
    //     }
    // }
}


void MPI::Solve(double *matrix, double *rhs, double *solution,
                int rows, int columns){
    int rank, size;
    // Obtain the rank of the current thread and the size of the communicator
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
   
   // Broadcast the parameters from rank 0 to other threads
    MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&columns, 1, MPI_INT, 0, MPI_COMM_WORLD);
    // Need to apply for space before broadcasting matrix, rhs and solution for rank>0
    if(rank){
        matrix = new double[rows*columns];
        rhs = new double[rows];
        solution = new double[rows];
    }
    MPI_Bcast(matrix, rows*columns, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(rhs, rows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
   
    // If you would like to run ForwardELimination_v1, please comment the following line
    MPI::ForwardElimination_v2(matrix, rhs, rows, columns, rank, size);
    // If you would like to run ForwardELimination_v1, please uncomment the following line
    // MPI::ForwardElimination_v1(matrix, rhs, rows, columns, rank, size);

    // ----------------------------------------------------------------------------------------------------
    // The BackwardSubstitution is parallized. If you would like to run unparallized
    // BackwardSubstitution, please comment the following line
    MPI::BackwardSubstitution(matrix, rhs, solution, rows, columns, rank, size);
    // If you would like to run unparallized BackwardSubstitution, please uncomment the following 3 lines
    
    // if(rank==0){
    //     MPI::BackwardSubstitution_rank0(matrix, rhs, solution, rows, columns);
    // }
    
    // ----------------------------------------------------------------------------------------------------

    // Release the space of matrix, rhs and solution for rank>0
    if(rank){
        delete[] matrix;
        delete[] rhs;
        delete[] solution;
    }
}
