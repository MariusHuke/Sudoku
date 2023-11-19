#include "Generation.hpp"
#include "Sudoku.hpp"
#include<chrono>

//read the sudokus from a given vector of paths
std::vector<Sudoku<int>> readfiles(std::vector<std::string> input_paths, int number_of_sudokus){
    std::fstream fstream;
    std::string tp;
    std::vector<Sudoku<int>> sudokus;
    for(int i = 0; i < input_paths.size();i++){ //iterate over paths
        fstream.open(input_paths[i],std::ios::in);
        int howmany = 0;
        while(getline(fstream,tp)){
            if(tp[0]=='#') continue; //ignore comments 
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

//read the 25x25 file 
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


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////TESTCASES////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//testcase using two_point_crossover, collision_fitness and stochastic_universal_sampling
void testcase(std::vector<Sudoku<int>> sudokus, int population_size, int selection_rate, int mutation_rate){
    std::vector<int> numbergens = {};
    std::vector<int> elapsedtime = {};
    float lastbest;
    int failedgens;
    for(int i = 0; i < sudokus.size(); i++){
        int generations = 0;
        lastbest = std::numeric_limits<float>::max();
        failedgens = 0;
        auto start = std::chrono::high_resolution_clock::now();
        Generation generation = Generation(sudokus[i],population_size);
        generation.fitness(true);
        while(true){
            generation.mutate(mutation_rate);
            generation.crossover(true); //two_point_crossover
            generation.fitness(true); //collision_fitness
            generation.selection(selection_rate,false); //stochastic_universal_sampling
            generations++;
            float bestvalue = generation.get_best();
            if (bestvalue >= lastbest){ 
                if(failedgens++ > 25){
                    generations = -1;
                    break;
                }
            }
            lastbest = bestvalue;
            failedgens = 0;
            if (bestvalue == 0){ //if solution is found
                break;
            }
        }
        auto stopclock = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stopclock-start);
        std::cout << "Sudoku " << i << " solved in " << generations << " generations and " << duration.count() << " milliseconds" << std::endl;
        numbergens.push_back(generations);
        elapsedtime.push_back(duration.count());
    }
    for(int i = 0; i < sudokus.size(); i++){
        std::cout << i << " " << numbergens[i] << " " << elapsedtime[i] << std::endl;
    }
}

int main(){
    std::vector<std::string> input_paths= {"data/testdata"}; //,data/puzzles0_kaggle"data/puzzles4_forum_hardest_1905"};
    std::vector<Sudoku<int>> sudokus = readfiles(input_paths, 30); //10*(easy,medium,hard,expert)
    testcase(sudokus, 1002, 20, 100);
    std::cout << "\n\n";
    return 1;
}