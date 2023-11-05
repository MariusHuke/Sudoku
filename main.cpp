#include "Generation.hpp"
#include "Sudoku.hpp"

//read in the sudoku from a given vector of paths
std::vector<Sudoku> readfiles(std::vector<std::string> input_paths, int number_of_sudokus){
    std::fstream fstream;
    std::string tp;
    std::vector<Sudoku> sudokus;
    
    for(int i = 0; i < input_paths.size();i++){
        fstream.open(input_paths[i],std::ios::in);
        int howmany = 0;
        while(getline(fstream,tp)){
            if(tp[0]=='#') continue;
            std::vector<int*> tmp = {}; 
            for(int ii = 0; ii < tp.length(); ii++){
                if(tp[ii]=='.') tmp.push_back(new int(0));
                else tmp.push_back(new int(tp[ii]-'0'));
            }
            sudokus.push_back(Sudoku(tmp));
            howmany++;
            if(howmany > number_of_sudokus) break;
        }
        fstream.close();
    }
    return sudokus;
}

int main(){
    std::vector<std::string> input_paths= {"data/puzzles0_kaggle"}; //,"data/puzzles4_forum_hardest_1905"};
    std::vector<Sudoku> sudokus = readfiles(input_paths, 1);
    Generation generation = Generation(sudokus[0],3);
    generation.print_values();
    std::vector<Sudoku> children = generation.nparents_npoints_crossover(3);
    for(int i = 0; i < children.size(); i++){
        children[i].print();
    }
    /*generation.fitness();
    generation.print_fitness();*/
    std::cout << "\n\n";
    return 1;
}