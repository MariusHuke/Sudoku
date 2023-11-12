#include "Generation.hpp"
#include "Sudoku.hpp"
#include<string>
#include<iostream>
//https://github.com/chinyan/Genetic-Algorithm-based-Sudoku-Solver/blob/master/GA_Sudoku_Solver.py#L394
//read in the sudoku from a given vector of paths
std::vector<Sudoku<int>> readfiles(std::vector<std::string> input_paths, int number_of_sudokus){
    std::fstream fstream;
    std::string tp;
    std::vector<Sudoku<int>> sudokus;
    for(int i = 0; i < input_paths.size();i++){
        fstream.open(input_paths[i],std::ios::in);
        int howmany = 0;
        while(getline(fstream,tp)){
            if(tp[0]=='#') continue;
            std::vector<int> tmp = {}; 
            for(int ii = 0; ii < tp.length(); ii++){
                if(tp[ii]==' ') continue;
                if(tp[ii]=='.' || tp[ii]=='0') tmp.push_back(0);
                else tmp.push_back((tp[ii]-'0'));
            }
            sudokus.push_back(Sudoku<int>(tmp));
            howmany++;
            if(howmany > number_of_sudokus) break;
        }
        fstream.close();
    }
    return sudokus;
}

std::vector<Sudoku<int>> readfiles25(std::vector<std::string> input_paths, int number_of_sudokus){
    std::fstream fstream;
    std::string tp;
    std::vector<Sudoku<int>> sudokus;
    for(int i = 0; i < input_paths.size();i++){
        fstream.open(input_paths[i],std::ios::in);
        int howmany = 0;
        std::string tmpstring;
        while(getline(fstream,tp)){
            std::vector<int> tmp = {};
            if (tp[0] == '#') continue; 
            for(int ii = 0; ii < tp.length(); ii++){
                if(tp[ii]==' '){
                    tmp.push_back(std::stoi(tmpstring));
                    tmpstring.clear();
                }
                else{
                    tmpstring += tp[ii];
                }
            }
            tmp.push_back(std::stoi(tmpstring));
            sudokus.push_back(Sudoku<int>(tmp));
            howmany++;
            if(howmany > number_of_sudokus) break;
        }
        fstream.close();
    }
    return sudokus;
}

int main(){
    std::vector<std::string> input_paths= {"25x25"}; //,data/puzzles0_kaggle"data/puzzles4_forum_hardest_1905"};
    std::vector<Sudoku<int>> sudokus = readfiles25(input_paths, 1);
    //initialize
    std::vector<int> results = {};  
    for(int i = 0; i < sudokus.size(); i++){
        int ii = 0;
        Generation generation = Generation(sudokus[i],25);
        generation.fitness(true);
        while(!generation.print_best()){
            std::cout << "Generation" << ii++ << "\n";
            generation.diagonal_crossover();
            generation.fitness(true);
            //generation.punish_same(20);
            generation.selection(50,false);
            generation.mutate(true);
            generation.fitness(true);
            if (i == 200){
                results.push_back(-1);
                break;
            }
        }
        results.push_back(ii);
    }
    for (int i = 0; i < results.size(); i++){
        std::cout << results[i] << " ";
    }
    return 1;
}