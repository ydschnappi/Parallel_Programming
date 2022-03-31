//
// Created by shubham on 26.05.21.
//

#ifndef TEST_INTERFACE_H
#define TEST_INTERFACE_H

namespace Parallel{
    void Solve(int argc, char** argv,
               float& sequential_runtime, float& parallel_runtime, int& print_rank);
}

#endif //TEST_INTERFACE_H
