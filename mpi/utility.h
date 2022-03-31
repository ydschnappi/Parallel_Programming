//
// Created by shubham on 14.05.21.
//

#ifndef TEST_UTILITY_H
#define TEST_UTILITY_H
#include <string>
#include <iostream>

namespace Utility{
    void InitializeArray(std::string& matrix_name, std::string& rhs_name,
                         double* matrix, double* rhs, double* solution, int rows, int columns,
                         bool load_ref=false, std::string* ref_name= nullptr, double* ref= nullptr);
    void ParseFilesNames(int argc, char** argv, std::string& matrix_name, std::string& rhs_name,
                         int* rows, int* columns, bool load_ref=false, std::string* ref_name = nullptr);
    void ClearBuffer(double* array);
    void PrintSolution(double* solution, int rows, int columns);

    template <typename T>
    void CompareBuffer(T* buf1, T* buf2, int size, T eps){
        for (int i=0; i<size; i++){
            if(std::abs(buf1[i] - buf2[i])>eps){
                std::cout<<"i = "<<i<<": buf1[i] = "<<buf1[i]<<", buf2[i] = "<<buf2[i]<<std::endl;
                delete[] buf1;
                delete[] buf2;
                exit(EXIT_FAILURE);
            }
        }
    }

}

#endif //TEST_UTILITY_H
