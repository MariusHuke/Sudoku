#ifndef GENERATION_HPP
#define GENERATION_HPP
#include "Sudoku.hpp"
#include <cassert>
#include <memory>
#include <cmath>

class Generation
{
public:
    // default constructor
    Generation(){
        population_size = 0;
        population = std::vector<Sudoku<int>>(0);
        fitness_sudokus = std::vector<Sudoku<float>>(0);
        fitness_sums = std::vector<float>(0);
    }

    // initialize a generation with a given population size by copying the given into every Candidate
    Generation(Sudoku<int> sudoku, int population_size, bool which=true){
        this->population_size = population_size;
        this->wished_population_size = population_size;
        this->original = sudoku;
        this->population = std::vector<Sudoku<int>>(0);
        this->fitness_sudokus = std::vector<Sudoku<float>>(0);
        this->fitness_sums = std::vector<float>(population_size,0);
        for (int i = 0; i < population_size; i++)
        {
            this->population.push_back(Sudoku<int>(sudoku));
            this->fitness_sudokus.push_back(Sudoku<float>(sudoku.size));
        }
	    if(which) initialize_random();
	    else initialize_smart();
    }

    //print all Sudokus of this Generation
    void print_values()
    {
        for (auto it = population.begin(); it != population.end(); it++)
        {
            it->print();
        }
    }

    //print Sudoku at pos i in Generation
    void print_value(int i)
    {
        population[i].print();
    }

    //get value at pos i in Generation
    Sudoku<int> get_value(int i)
    {
        return population[i];
    }

    //print all fitness values in Generation
    void print_fitness()
    {
        for (auto  it = fitness_sudokus.begin(); it != fitness_sudokus.end(); it++)
        {
            it->print();
        }
    }

    //print Fitness at pos i in Generation
    void print_fitness(int i)
    {
        fitness_sudokus[i].print();
    }

    //get fitness at pos i in Generation
    Sudoku<float> get_fitness(int i)
    {
        return fitness_sudokus[i];
    }

    //print all fitness_sums
    void print_fitness_sums()
    {
        for(auto it = fitness_sums.begin();it != fitness_sums.end();it++){
            std::cout << *it;
        }
    }

    //get fitness_sums at pos i in Generation
    std::vector<float> get_fitness_sums()
    {
        return fitness_sums;
    }

    //print best individual and return true if a solution is found 
    bool print_best(){
        float avg = 0;
        for(auto it = fitness_sums.begin();it != fitness_sums.end();it++){
            avg += *it;
        }
        avg /= fitness_sums.size();
        auto best = std::min_element(fitness_sums.begin(), fitness_sums.end());
        int bestindex = std::distance(fitness_sums.begin(), best);
        float bestvalue = *best;
        std::cout << bestvalue << " at: " << bestindex << " with average:" << avg << "\n";
        if(bestvalue == 0){
            population[bestindex].print();
            return 1;
        }
        return 0;
    }

    //return best fitness values of the generation
    float get_best(){
        auto best = std::min_element(fitness_sums.begin(), fitness_sums.end());
        return *best;
    }

    //check if average == best (cause that is a local minimum)
    //0 == dont stop, 1 == 0 reached, 2 == (best==avg)
    int stop(){
        auto best = std::min_element(fitness_sums.begin(), fitness_sums.end());
        float bestvalue = *best;
        if(bestvalue == 0){
            return 0;
        }
        return 1;
    }

    //calculate fitness (bool decides method)
    void fitness()
    {
        calculate_collision_fitness();
    }

    //mutation
    void mutate()
    {
        collision_mutation();
    }

    //selection 
    void selection(int keeping_percentage, bool which = true){
        if(which) stochastic_universal_sampling((float)keeping_percentage);
        else strongest_selection(keeping_percentage);
    }

    //diagonal crossover (or special case 2 point, which makes sense for 9x9 sudokus)
    void crossover(bool which = true)
    {
        if(which) crossover(original.segment_row_length);
        else crossover(2);
    }

    //can be called to punish sudokus that are vastly the same
    //not really useful 
    void punish_same(int punishment_strength){
        //count how often each number is present in each square
        std::vector<std::vector<float>> counting = std::vector<std::vector<float>>(population[0].size, std::vector<float>(population[0].row_length, 0));
        for(int i = 0; i < population_size; i++){
            for(int ii = 0; ii < population[i].size; ii++){
                counting[ii][*(population[i].row_representation[ii])-1]+=1/(float)population_size;
            }
        }
        //punish fitness
        std::vector<float> addedsums = std::vector<float>(population_size, 0);
        for(int i = 0; i < population_size; i++){
            float addedsum = 0;
            for(int ii = 0; ii < population[i].size; ii++){
                if(*(original.row_representation[ii]) == 0){
                    addedsum += counting[ii][*(population[i].row_representation[ii])-1];
                }
            }
            addedsums[i] = addedsum;
        }
        //minmax normalization on addedsums and multiplication with punishment_strenght
        float min = *std::min_element(addedsums.begin(), addedsums.end());
        float max = *std::max_element(addedsums.begin(), addedsums.end());
        for(int i = 0; i < population_size; i++){
            fitness_sums[i] += (addedsums[i] - min) / (max - min)*punishment_strength;
        }
    }

private:

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////HELPERS//////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    //used for getting lists that contain all elements that could be filled in
    std::vector<float> reset_list(int size)
    {
        std::vector<float> list = {};
        for (int i = 0; i < size; i++)
        {
            list.push_back(i + 1);
        }
        return list;
    }

    //check whether or not there are collision in the row (true == no)
    bool checkrow(Sudoku<int> sudoku, int row, int number){
        for(int i = 0; i < sudoku.row_length; i++){
            if(*(sudoku.row_representation[row*sudoku.row_length+i]) == number) return false;
        }
        return true; 
    }

    //check whether or not there are collision in the coloumn (true == no)
    bool checkcoloumn(Sudoku<int> sudoku, int coloumn, int number){
        for(int i = 0; i < sudoku.row_length; i++){
            if(*(sudoku.row_representation[i*sudoku.row_length+coloumn]) == number) return false;
        }
        return true; 
    }

    //check whether or not there are collision in the grid (true == no)
    bool checkgrid(Sudoku<int> sudoku, int grid, int number){
        for(int i = 0; i < sudoku.row_length; i++){
            if(*(sudoku.grid_representation[grid][i]) == number) return false;
        }
        return true; 
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////INITIALIZATION///////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    // create a initial generation by filling the empty fields with random numbers (grid representation)
    void initialize_random()
    {
        // create random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        // get grid
        for (auto it = population.begin(); it != population.end(); it++){
            std::vector<std::vector<std::shared_ptr<int>>> grid = it->grid_representation;
            for (int i = 0; i < grid.size(); i++)
            {
                std::vector<float> list = reset_list(grid.size());
                std::shuffle(list.begin(), list.end(), gen);
                //find present elements in grid and swap them to the position they belong
                for (int ii = 0; ii < grid[i].size(); ii++){
                    if (*(grid[i][ii]) == 0) //because at initialization all empty sqaures are 0
                        continue;
                    auto iit = std::find(list.begin(), list.end(), *(grid[i][ii]));
                    int tmp = (*iit);
                    (*iit) = list[ii];
                    list[ii] = tmp;
                }
                //copy list into grid
                for (int ii = 0; ii < grid[i].size(); ii++){
                    *(grid[i][ii]) = list[ii];
                }
            }
        }
    }

    //initialization also takes row/coloumn collision as a factor
    //which pair of collision is choose randomly between checkrow && (checkcoloumn || checkgrid)
    void initialize_smart(){
        // create random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 1);
        for (auto it = population.begin(); it != population.end(); it++){ //iterate over population
            bool horizontal = dist(gen);
            std::vector<std::vector<std::shared_ptr<int>>> grid = it->grid_representation;
            std::vector<float> gridlist = reset_list(grid.size());
            for (int i = 0; i < grid.size(); i++){ //iterate over subgrids
                //chose a random element from grid
                std::uniform_int_distribution<int> griddist(0, gridlist.size()-1);
                int gridpos = griddist(gen);
                int gridposvalue = gridlist[gridpos]-1;
                gridlist.erase(gridlist.begin()+gridpos);
                std::vector<std::vector<int>> possibles = std::vector<std::vector<int>>(it->segment_row_length, std::vector<int>(0));
                std::vector<std::vector<int>> how_common = std::vector<std::vector<int>>(it->segment_row_length,std::vector<int>(it->row_length, 0));
                //find possibles for each row of the grid
                for (int ii = 0; ii < possibles.size(); ii++){ //iterate over rows of subgrid
                    for(int iii = 0; iii < it->row_length; iii++){ //possible elements for the row
                        if(checkrow(*it, ii+it->segment_row_length*(gridposvalue/it->segment_row_length), iii+1) && horizontal && checkgrid(*it, gridposvalue, iii+1)){
                            possibles[ii].push_back(iii+1);
                            for(int iv = 0; iv < it->segment_row_length; iv++){
                                if(iv != ii) how_common[iv][iii]++;
                            }
                        }
                        else if (checkcoloumn(*it, ii+(gridposvalue%it->segment_row_length)*it->segment_row_length, iii+1) && !horizontal && checkgrid(*it, gridposvalue, iii+1)){
                            possibles[ii].push_back(iii+1);
                            for(int iv = 0; iv < it->segment_row_length; iv++){
                                if(iv != ii) how_common[iv][iii]++;
                            }
                        }
                    }
                }
                //sort by possibles by how common elements are in the others
                for(int ii = 0; ii < possibles.size();ii++){
                    std::sort(possibles[ii].begin(), possibles[ii].end(),
                        [&how_common,ii](const int &a, const int &b)
                        {
                            return how_common[ii][a] > how_common[ii][b];
                        });
                }
                std::uniform_int_distribution<int> startdist(0, it->segment_row_length-1);
                int insertstart = startdist(gen);
                //fill grid with possibles
                for (int ii = 0; ii < grid[gridposvalue].size(); ii++){
                    //find smallest possibles
                    int size = std::numeric_limits<int>::max();
                    int pos = 0;
                    std::vector<int> first = std::vector<int>(possibles.size(), 0);
                    for(int iii = 0; iii < possibles.size(); iii++){
                        if(possibles[iii].size() < size && possibles[iii].size() != 0){
                            pos = iii;
                            size = possibles[iii].size();
                        }
                    }
                    //fill grid with one element
                    if(horizontal){ //horizontal
                        if(possibles[pos].size() == 0) continue;
                        //find first empty position
                        while(*(grid[gridposvalue][pos*it->segment_row_length+((first[pos]+insertstart)%it->segment_row_length)]) != 0){
                            first[pos]++;
                            if(first[pos] == it->segment_row_length){
                                possibles[pos] = std::vector<int>(0);
                                break;
                            }
                        }
                        if(first[pos] == it->segment_row_length) continue;
                        //put into first pos
                        *(grid[gridposvalue][pos*it->segment_row_length+((first[pos]+insertstart)%it->segment_row_length)]) = possibles[pos][0];
                        //erase used number from all possibles
                        for(int iii = 0; iii < possibles.size(); iii++){
                            possibles[iii].erase(std::remove(possibles[iii].begin(), possibles[iii].end(), *(grid[gridposvalue][pos*it->segment_row_length+((first[pos]+insertstart)%it->segment_row_length)])), possibles[iii].end());
                        }
                    }
                    else{ //vertical
                        if(possibles[pos].size() == 0) continue;
                        //find first empty position
                        while(*(grid[gridposvalue][((first[pos]+insertstart)%it->segment_row_length)*it->segment_row_length+pos]) != 0){
                            first[pos]++;
                            if(first[pos] == it->segment_row_length){
                                possibles[pos] = std::vector<int>(0);
                                break;
                            }
                        }
                        if(first[pos] == it->segment_row_length) continue;
                        //put into first pos
                        *(grid[gridposvalue][((first[pos]+insertstart)%it->segment_row_length)*it->segment_row_length+pos]) = possibles[pos][0];
                        //erase used number from all possibles 
                        for(int iii = 0; iii < possibles.size(); iii++){
                            possibles[iii].erase(std::remove(possibles[iii].begin(), possibles[iii].end(), *(grid[gridposvalue][((first[pos]+insertstart)%it->segment_row_length)*it->segment_row_length+pos])), possibles[iii].end());
                        }
                    }
                }
                //fill leftovers (collides with row, but leftovers are rare)
                for(int ii = 0; ii < grid.size();ii++){
                    if(*(grid[gridposvalue][ii]) == 0){
                        for(int iii = 1; iii < 10; iii++){
                            if(checkgrid(*it, gridposvalue, iii)){
                                *(grid[gridposvalue][ii]) = iii;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////CROSSOVER////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    //crossovers are done at the horitzontal borders or grid rows
    //for 9x9 sudoku n=2 ist 2point crossover which swaps just the middle row
    //otherwise e.g. with 16x16 and 2 point only the second row would be given by the other parent
    void crossover(int n)
    {
        assert(population_size % n == 0);
        int copy_element_size = (population[0].row_length) * (population[0].segment_row_length);
        std::vector<Sudoku<int>> children = std::vector<Sudoku<int>>(population_size);
        std::vector<Sudoku<int>> mutated = {};
        for (int i = 0; i < population_size; i += n)
        { // iterate over population
            for (int ii = 0; ii < n; ii++)
            { // initialize children by copying parents
                children[i + ii] = Sudoku<int>(population[i + ii]);
            }
            for (int ii = 0; ii < n; ii++)
            { // iterate over children
                int parent = ii + 1;
                if (parent == n) parent = 0;
                while (parent != ii)
                {
                    for (int iii = 0; iii < copy_element_size; iii++)
                    { // iterate over rows
                        *(children[i + ii].row_representation[(ii + parent) % n * copy_element_size + iii]) 
                        = *(population[i + parent].row_representation[(ii + parent) % n * copy_element_size + iii]);
                    }
                    parent++;
                    if (parent == n)
                        parent = 0;
                }
            }
        }
        for (int i = 0; i < population_size; i++)
        {
            population.push_back(children[i]);
        }
        population_size = population.size();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////FITNESS//////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    //calculate fitness based on number of collisions
    void calculate_collision_fitness()
    {
        fitness_sums = std::vector<float>(population_size, 0);
        fitness_sudokus = std::vector<Sudoku<float>>(0);
        for (int i = 0; i < population.size(); i++)
        {
            fitness_sudokus.push_back(Sudoku<float>(population[i].size));
        }
        for (int i = 0; i < population.size(); i++){
            int sum = 0;
            //calculate collision with others
            std::vector<std::vector<std::shared_ptr<float>>> hashmap;
            //rows
            for (int ii = 0; ii < population[i].size; ii += population[i].row_length)
            {
                hashmap = std::vector<std::vector<std::shared_ptr<float>>>(population[i].row_length, std::vector<std::shared_ptr<float>>(0));
                for (int iii = 0; iii < population[i].row_length; iii++)
                {
                    hashmap[*(population[i].row_representation[ii + iii]) - 1].push_back(fitness_sudokus[i].row_representation[ii + iii]);
                }
                for (int iii = 0; iii < hashmap.size(); iii++)
                {
                    for (int iv = 0; iv < hashmap[iii].size(); iv++)
                    {
                        *hashmap[iii][iv] += (hashmap[iii].size() > 1);
                        sum += (hashmap[iii].size() > 1);
                    }
                }
            }
            // coloumns
            for (int ii = 0; ii < population[i].row_length; ii++)
            {
                hashmap = std::vector<std::vector<std::shared_ptr<float>>>(population[i].row_length, std::vector<std::shared_ptr<float>>(0));
                for (int iii = 0; iii < population[i].row_representation.size(); iii += population[i].row_length)
                {
                    hashmap[*(population[i].row_representation[ii + iii]) - 1].push_back(fitness_sudokus[i].row_representation[ii + iii]);
                }
                for (int iii = 0; iii < hashmap.size(); iii++)
                {
                    for (int iv = 0; iv < hashmap[iii].size(); iv++)
                    {
                        *hashmap[iii][iv] += ((hashmap[iii].size()) > 1);
                        sum += (hashmap[iii].size() > 1);
                    }
                }
            }
            fitness_sums[i] = sum;
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////SELECTION////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////


    //helper struct to easily sort sudokus
    struct item
    {
        float fitness_sum;
        Sudoku<int> population_element;
        Sudoku<float> fitness_element;
    };

    //simply selecting just the elements with least (=best) fitness
    void strongest_selection(int keeping_percentage)
    {
        assert(keeping_percentage <= 100);
        //transform to items
        std::vector<item> fitness_values = std::vector<item>(0); 
        for (int i = 0; i < fitness_sums.size(); i++)
        {
            fitness_values.push_back({fitness_sums[i], population[i], fitness_sudokus[i]});
        }
        //sort (biggest fitness value last)
        std::sort(fitness_values.begin(), fitness_values.end(),
                  [](const item &a, const item &b)
                  {
                      return a.fitness_sum < b.fitness_sum;
                  });
        //calculate number of deleted items
        int delete_size = (population_size * (100-keeping_percentage)) / 100;
        std::vector<item> new_population(fitness_values.begin(), fitness_values.end() - delete_size);
        //clear old population and append selected candidates 
        population.clear();
        fitness_sudokus.clear();
        fitness_sums.clear();
        for (int i = 0; i < new_population.size(); i++)
        {
            population.push_back(new_population[i].population_element);
            fitness_sudokus.push_back(new_population[i].fitness_element);
            fitness_sums.push_back(new_population[i].fitness_sum);
        }
        population_size = population.size();
    }

    //using stochastic universal sampling selection technique
    void stochastic_universal_sampling(float keeping_percentage){
        assert(keeping_percentage <= 100);

        //min-max normalization of fitness_sums
        float sum = 0;
        float max = *std::max_element(fitness_sums.begin(), fitness_sums.end());
        float min = *std::min_element(fitness_sums.begin(), fitness_sums.end());
        std::vector<float> normalized_fitness = {};
        for (int i = 0; i < fitness_sums.size(); i++)
        {
            normalized_fitness.push_back((max - fitness_sums[i] - min) / (max - min));
            sum += normalized_fitness[i];
        }

        //prepare roulette wheel
        std::vector<float> roulette_wheel = std::vector<float>(0); 
        float pointer = 0;
        for(int i = 0; i < normalized_fitness.size(); i++){
            roulette_wheel.push_back(normalized_fitness[i] / sum + pointer);  //(1-) so that worst is smallest
            pointer += normalized_fitness[i] / sum;
        }
        //get starting position
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(0, 1);
        pointer = dist(rd);

        //perform selection
        std::vector<item> new_population = std::vector<item>(0);
        int i=0;
        while(i < ((float)(population_size*keeping_percentage)/100)){
            int index = 0;
            while(roulette_wheel[index] < pointer){
                index++;
            }
            new_population.push_back({fitness_sums[index], population[index], fitness_sudokus[index]});
            pointer += 1 / ((float)(population_size*keeping_percentage)/100);
            if(pointer >= 1) pointer-=1;
            i++;
        }

        //clear population and add selecteds
        population.clear();
        fitness_sudokus.clear();
        fitness_sums.clear();
        for (int i = 0; i < new_population.size(); i++)
        {
            population.push_back(new_population[i].population_element);
            fitness_sudokus.push_back(new_population[i].fitness_element);
            fitness_sums.push_back(new_population[i].fitness_sum);
        }
        population_size = population.size();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////MUTATION////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    // randomly choose and swap elements that have a fitness_value higher than zero
    // and are therefore colliding with others (calculate_collision_fitness)
    // or have are quite far from the "perfect values" (calculate_all_fitness)
    void collision_mutation()
    {
        std::vector<Sudoku<int>> mutated = std::vector<Sudoku<int>>(wished_population_size);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 100);
        for (int i = 0; i < wished_population_size; i++)
        {
            int parentpos = i % population_size;
            Sudoku<int> mutation = Sudoku<int>(population[parentpos].size);
            for (int ii = 0; ii < population[parentpos].row_length; ii++)
            { // iterate over grids
                std::vector<int> swaps = {};
                for (int iii = 0; iii < population[parentpos].row_length; iii++)
                { // iterate over elements of grids and append to swaps list 
                    if (*(original.grid_representation[ii][iii]) != 0)
                    {
                        *(mutation.grid_representation[ii][iii]) = *(original.grid_representation[ii][iii]);
                    }
                    else if (0 < *(fitness_sudokus[parentpos].grid_representation[ii][iii]) || 11 > dist(gen))
                    {
                        swaps.push_back(iii);
                    }
                    else
                    {
                        *(mutation.grid_representation[ii][iii]) = *(population[parentpos].grid_representation[ii][iii]);
                    }
                }
                //shuffle and fill elements into grid
                std::shuffle(swaps.begin(), swaps.end(), gen);
                if (swaps.size() != 0)
                {
                    for (int iii = 0; iii < swaps.size() - 1; iii += 2)
                    {
                        *(mutation.grid_representation[ii][swaps[iii]]) = *(population[parentpos].grid_representation[ii][swaps[iii + 1]]);
                        *(mutation.grid_representation[ii][swaps[iii + 1]]) = *(population[parentpos].grid_representation[ii][swaps[iii]]);
                    }
                    if (swaps.size() % 2 != 0)
                    {
                        *(mutation.grid_representation[ii][swaps[swaps.size() - 1]]) = *(population[parentpos].grid_representation[ii][swaps[swaps.size() - 1]]);
                    }
                }
            }
            mutated[i] = mutation;
        }
        population = mutated;
        fitness_sums.clear();
        fitness_sudokus = std::vector<Sudoku<float>>(0);
        population_size = population.size();
    }

    Sudoku<int> original;
    std::vector<float> fitness_sums; //fitness of a whole sudoku
    std::vector<Sudoku<float>> fitness_sudokus; //representation of fitness values for each individual cell
    std::vector<Sudoku<int>> population; //generated "solutions"
    int population_size;
    int wished_population_size; //originaly set size 
};
#endif
