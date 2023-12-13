#include "Solver.hpp"

int main(){
    std::vector<std::string> input_paths= {"../data/testdata"}; //10*(easy,medium,hard,expert)
    Solver solver = Solver();
    std::vector<Sudoku<int>> sudokus = solver.readfiles(input_paths, 40);
    std::ofstream outFile("a.txt");
    solver.testcase(sudokus, 100, 20);
    return 1;
}
