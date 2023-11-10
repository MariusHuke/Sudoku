#include "Generation.hpp"
#include "Sudoku.hpp"

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

int main(){
    std::vector<std::string> input_paths= {"data/puzzles0_kaggle"}; //,data/puzzles0_kaggle"data/puzzles4_forum_hardest_1905"};
    std::vector<Sudoku<int>> sudokus = readfiles(input_paths, 1);
    //initialize
    Generation generation = Generation(sudokus[0],2);
    generation.fitness(true);
    generation.print_value(0);
    generation.print_fitness(0);
    /*generation.mutate(false);
    generation.fitness(true);
    generation.print_value(0);
    generation.print_fitness(0);*/
    /*generation.roulette_wheel_selection(20);
    generation.mutate(true);
    generation.print_value(0);*/
    /*generation.fitness(true);
    std::vector<float> fitness = generation.get_fitness();
    auto best = std::min_element(fitness.begin(), fitness.end());
    int bestindex = std::distance(fitness.begin(), best);
    int bestvalue = *best;
    int i = 0;
    int selection = 20;
    bool fitnessval = true;
    while(bestvalue!=0){
        std::cout << "Generation" << i++ << "\n";
        generation.two_point_crossover();
        generation.fitness(fitnessval); //todo: make a fitnes function that just initialzes with 0
        generation.punish_same();

        //std::cout << bestvalue << " at: " << bestindex << "\n";
        generation.selection(50);
        generation.mutate(false);
        generation.fitness(fitnessval); //the first few off the "sames" cant be punished
        fitness = generation.get_fitness();
        best = std::min_element(fitness.begin(), fitness.end());
        bestindex = std::distance(fitness.begin(), best);
        bestvalue = *best;
        //std::cout << bestvalue << " at: " << bestindex << "\n";
        if (i%10 == 0){
            generation.print_value(bestindex);
            generation.print_fitness(bestindex);
            std::cout << bestvalue << " at: " << bestindex << "\n";
        } 
        if (i == 2000){
            generation = Generation(sudokus[0],20);
            i = 0;
            generation.fitness(true);
        }
    }
    generation.print_value(bestindex);*/

    return 1;
}