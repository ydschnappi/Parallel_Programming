//
// Created by shubham on 27.05.21.
//

#include "omp_interface.h"
#include "utility.h"
#include <iostream>
#include "serial.h"
#include <chrono>
#include "ompge.h"

void Parallel::OmpSolve(int argc, char **argv, float &parallel_runtime) {
    ///////////////////////////////////////////////////////////////////////////////
    /// Uncommenting the commented part will compare //////////////////////////////
    /// the sequential solution to parallel ///////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////

    int rows, columns;
//    double *matrixSeq, *rhsSeq, *solutionSeq, *reference;
    double *matrixPar, *rhsPar, *solutionPar;
    std::string matrix_name, rhs_name, ref_name;

    Utility::ParseFilesNames(argc, argv, matrix_name, rhs_name, &rows, &columns);

//    matrixSeq = new double[(rows) * (columns)];
//    rhsSeq = new double[(rows)];
//    solutionSeq = new double[(columns)];


    matrixPar = new double[(rows) * (columns)];
    rhsPar = new double[(rows)];
    solutionPar = new double[(columns)];

    Utility::InitializeArray(matrix_name, rhs_name,
                             matrixPar, rhsPar, solutionPar, rows, columns);

//    std::copy(matrixPar, matrixPar + rows*columns, matrixSeq);
//    std::copy(rhsPar, rhsPar + rows, rhsSeq);
//    std::copy(solutionPar, solutionPar + columns, solutionSeq);

//    Serial::Solve(matrixSeq, rhsSeq, solutionSeq, rows, columns);

    auto start = std::chrono::steady_clock::now();
    OMP::Solve(matrixPar, rhsPar, solutionPar, rows, columns);
    auto end = std::chrono::steady_clock::now();
    parallel_runtime = std::chrono::duration<float>(end - start).count();


//    delete[] matrixSeq;
//    delete[] rhsSeq;
    delete[] matrixPar;
    delete[] rhsPar;
    Utility::PrintSolution(solutionPar, rows, columns);

//    Utility::CompareBuffer<double>(solutionSeq, solutionPar, columns, 1e-6);
//    delete[] solutionSeq;
    delete[] solutionPar;
}
