//
// Created by shubham on 14.05.21.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <string>
#include "utility.h"

namespace Utility{
// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}
}
void Utility::ParseFilesNames(int argc, char** argv, std::string& matrix_name,
                              std::string& rhs_name, int* rows, int* columns,
                              bool load_ref, std::string* ref_name){
    if( argc != 2 ) {
        std::cerr <<"The base name of the input matrix and vector files must be given"<<std::endl;
        exit(-1);
    }
    matrix_name = argv[1];
    matrix_name = matrix_name.append(".mat");
    rhs_name = argv[1];
    rhs_name = rhs_name.append(".vec");

    std::ifstream matrix_file;
    matrix_file.open (matrix_name);
    std::string line;
    std::getline(matrix_file, line);
    std::istringstream iss(line);
    if(!(iss >> *rows >> *columns)){
        std::cerr << "unable to read rows and columns!" << std::endl;
        exit(EXIT_FAILURE);
    }
    if(load_ref){
        *ref_name = argv[1];
        *ref_name = (*ref_name).append(".sol");
    }
}

void ReadLine(std::string& line, double* array, int size){
    std::istringstream iss(line);
    for(int i=0; i<size; i++){
        if(!(iss >> array[i])){
            std::cerr << "unable to read line = "<<line << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

bool DimensionCheck(int matrow, int matcol, int rhssize){
    if(matrow==matcol && matcol==rhssize){
        return true;
    }
    else{
        return false;
    }
}

void Utility::InitializeArray(std::string& matrix_name, std::string& rhs_name,
                     double* matrix, double* rhs, double* solution, int rows, int columns,
                     bool load_ref, std::string* ref_name, double* ref){
    std::ifstream matrix_file, vector_file, ref_file;
    std::cout<<"READ:  Matrix   (A) file name: "<< matrix_name<<std::endl;
    std::cout<<"READ:  RHS      (b) file name: "<< rhs_name<<std::endl;

    matrix_file.open (matrix_name);
    std::string line;
    std::getline(matrix_file, line);
    std::istringstream iss(line);
    int r1, r2;
    if(!(iss >> r1 >> r2)){
        std::cerr << "unable to read rows and columns!" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (r1!=rows || r2!=columns){
        std::cerr << "Dimension mismatch between subsequent access to file" << std::endl;
        std::cerr <<"row x columns from 1st access = "<<rows<<" x "<< columns<<std::endl;
        std::cerr <<"row x columns from 2nd access = "<<r1<<" x "<< r2<<std::endl;
        exit(EXIT_FAILURE);
    }


    int rhs_size;
    iss.clear();
    vector_file.open(rhs_name);
    std::getline(vector_file, line);
    iss.str(line);
    if(!(iss >> rhs_size)){
        std::cerr << "unable to read number of row for rhs!" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(!DimensionCheck(rows, columns, rhs_size)){
        std::cerr << "Inconsistent Size" << std::endl;
        exit(EXIT_FAILURE);
    }

    for(int row = 0; row < rows; row++){
        std::getline(matrix_file, line);
        ReadLine(line, &matrix[row*(columns)], columns);
    }
    std::getline(vector_file, line);
    ReadLine(line, rhs, rows);

    if(load_ref){
        int ref_size;
        iss.clear();
        ref_file.open((*ref_name));
        std::getline(ref_file, line);
        iss.str(line);
        if(!(iss >> ref_size)){
            std::cerr << "unable to read number of row for ref!" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::getline(ref_file, line);
        ReadLine(line, ref, columns);
    }

}

void Utility::PrintSolution(double* solution, int rows, int columns){
    std::cout<<"Rows = "<<rows<<", Columns = "<<columns<<std::endl;
    for(int i=0; i<columns; ++i){
        std::cout<<solution[i]<<std::endl;
    }
}

void Utility::ClearBuffer(double* array){
    delete[] array;
}
