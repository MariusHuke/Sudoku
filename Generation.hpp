#ifndef GENERATION_HPP
#define GENERATION_HPP
#include "Sudoku.hpp"
#include <cassert>
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
    void print_values(){
        for(int i = 0; i < population.size(); i++){
            population[i].print();
        }
    }

    void print_fitness(){
        for(int i = 0; i < fitness_sudokus.size(); i++){
            fitness_sudokus[i].print();
        }
    }

    //getters
    std::vector<Sudoku> get_population(){
        return population;
    }

    std::vector<Sudoku> get_fitness_sudokus(){
        return fitness_sudokus;
    }

    void fitness(){
        calculate_fitness();
    }

    //crossovers are done at the borders or grid rows
    std::vector<Sudoku> nparents_npoints_crossover(int n = 2){
        assert(population_size%n==0);
        assert((population[0].size/n)%(population[0].row_length)*(population[0].segment_row_length)==0); //check whether or not points can be set at borders
        std::vector<Sudoku> children = {};
        for(int i = 0; i < population_size; i+=n){ //iterate over population
            for(int ii = 0; ii < n; ii++){ //initailize children by copying parents
                children.push_back(Sudoku(population[i+ii].get_copy()));
            }
            for(int ii = 0; ii < n; ii++){ //iterate over children
                for(int iii = 0; iii < n; iii++){ //iterate over parents
                    for(int iv = ; iv < population[i+ii].size/n; iv++){
                        *(children[ii].get_row_representation()[iii]) = *(population[i+ii].get_row_representation()[iii]);
                    }
                }
            }
        }
        return children;
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

    void calculate_fitness(){
        for(int i = 0; i < population.size(); i++){
            fitness_sudokus.push_back(Sudoku(population[i].get_row_representation().size()));
            fitness_sums.push_back(0);
            std::vector<std::vector<int*>> hashmap;
            //rows
            for(int ii = 0; ii < population[i].get_row_representation().size(); ii+=population[i].row_length){
                hashmap = std::vector<std::vector<int*>>(population[i].row_length,std::vector<int*>(0));
                for(int iii = 0; iii < population[i].row_length; iii++){
                    hashmap[*(population[i].get_row_representation()[ii+iii])-1].push_back(fitness_sudokus[i].get_row_representation()[ii+iii]);
                }
                for (int iii = 0; iii < hashmap.size(); iii++){
                    for(int iv = 0; iv < hashmap[iii].size(); iv++){
                        *hashmap[iii][iv] += hashmap[iii].size()-1;
                        fitness_sums[i] += hashmap[iii].size()-1;
                    }
                }
            }
            //coloumns
            for(int ii = 0; ii < population[i].row_length; ii++){
                hashmap = std::vector<std::vector<int*>>(population[i].row_length,std::vector<int*>(0));
                for(int iii = 0; iii < population[i].get_row_representation().size(); iii+=population[i].row_length){
                    hashmap[*(population[i].get_row_representation()[ii+iii])-1].push_back(fitness_sudokus[i].get_row_representation()[ii+iii]);
                }
                for (int iii = 0; iii < hashmap.size(); iii++){
                    for(int iv = 0; iv < hashmap[iii].size(); iv++){
                        *hashmap[iii][iv] += hashmap[iii].size()-1;
                        fitness_sums[i] += hashmap.size()-1;
                    }
                }
            }
        }
    }

    Sudoku original;
    std::vector<int> fitness_sums;
    std::vector<Sudoku> fitness_sudokus; //representation of fitness values for each individual is the same
    std::vector<Sudoku> population;
    int population_size;
};
#endif