#ifndef GENERATION_HPP
#define GENERATION_HPP
#include "Sudoku.hpp"

class Generation{
    public:
    //default constructor
    Generation(){
        population_size = 0;
        population = std::vector<Sudoku>(0);
    }

    //initailize a generation with a given population size
    Generation(Sudoku sudoku, int population_size){
        this->population_size = population_size;
        this->original = sudoku;
        for(int i = 0; i < population_size; i++){
            population.push_back(sudoku.get_copy());
        }
        initialize();
    }

    //print
    void print(){
        for(int i = 0; i < population.size(); i++){
            population[i].print();
        }
    }

    private:
    //resets the list to a ordered list of all elements that can be set
    std::vector<int> reset_list(int size){
        std::vector<int> list = {};
        for(int i = 0; i < size; i++){
            list.push_back(i+1);
        }
        return list;
    }

    //create a initial generation by filling the empty fields with random numbers (grid representation)
    void initialize(){
        //create random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        //get grid
        for(auto it = population.begin(); it != population.end(); it++){
            std::vector<std::vector<int*>> grid = it->get_grid_representation();
            for(int i = 0; i < grid.size(); i++){
                std::vector<int> list = reset_list(grid.size());
                std::shuffle(list.begin(),list.end(),gen);
                for(int ii = 0; ii < grid[i].size(); ii++){
                    if(*(grid[i][ii])==0) continue;
                    auto it = std::find(list.begin(),list.end(),*(grid[i][ii]));
                    int tmp = (*it);
                    (*it) = list[ii];
                    list[ii] = tmp;
                }
                for(int ii = 0; ii < grid[i].size(); ii++){
                    *(grid[i][ii]) = list[ii];
                }
            }
        }
    }

    //TODO: implement fitness function
    void calculate_fitness(Sudoku sudoku){

    }
    
    private:
    Sudoku original;
    std::vector<int> fitness_sums;
    std::vector<Sudoku> fitness_sudokus; //representation of fitness values for each individual is the same
    std::vector<Sudoku> population;
    int population_size;
};
#endif