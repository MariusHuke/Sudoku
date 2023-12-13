#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "Solver.hpp"

//export constructor ans step function so it can be used by the gui
PYBIND11_MODULE(Sudoku, m){
    pybind11::class_<Solver>(m, "Solver")
        .def(pybind11::init<int,int,std::string>())
        .def("step",&Solver::step);
}

//can be compiled to .so with
//c++ -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) ModuleExport.cpp -o Sudoku$(python3-config --extension-suffix)