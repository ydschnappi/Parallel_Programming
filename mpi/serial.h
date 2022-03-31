//
// Created by shubham on 14.05.21.
//

#ifndef TEST_SERIAL_H
#define TEST_SERIAL_H

namespace Serial{

    void Solve(double *matrix, double *rhs, double *solution,
              int rows, int columns);

    void SerialSolve(int argc, char **argv, float &sequential_runtime, int rank);

}
#endif //TEST_SERIAL_H
