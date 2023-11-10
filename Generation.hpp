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
    Generation(Sudoku<int> sudoku, int population_size){
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
        std::cout << "Generation initialized with " << population_size << " elements\nSudoku:\n";
        sudoku.print();
        initialize_smart();
    }

    //print all Sudokus of this Generation
    void print_values()
    {
        for (int i = 0; i < population.size(); i++)
        {
            population[i].print();
        }
    }

    //print Sudoku at pos i in Generation
    void print_value(int i)
    {
        population[i].print();
    }

    //print all fitness values in Generation
    void print_fitness()
    {
        for (int i = 0; i < fitness_sudokus.size(); i++)
        {
            fitness_sudokus[i].print();
        }
    }

    //print Fitness at pos i in Generation
    void print_fitness(int i)
    {
        fitness_sudokus[i].print();
    }

    //print all fitness_sums
    void print_fitness_sums()
    {
        for(auto it = fitness_sums.begin();it != fitness_sums.end();it++){
            std::cout << *it;
        }
    }

    //print best individual 
    void print_best(){
        auto best = std::min_element(fitness_sums.begin(), fitness_sums.end());
        int bestindex = std::distance(fitness_sums.begin(), best);
        float bestvalue = *best;
        std::cout << bestvalue << " at: " << bestindex << "\n";
    }

    //calculate fitness (bool decides method)
    void fitness(bool which)
    {
        if(which) calculate_collision_fitness();
        else calculate_all_fitness();
    }

    //mutate (bool decides methods)
    void mutate(bool intelligent = true)
    {
        if (intelligent)
            intelligent_mutation();
        else
            collision_mutation();
    }

    void selection(int keeping_percentage, bool which = true){
        if(which) stochastic_universal_sampling((float)keeping_percentage);
        else strongest_selection(keeping_percentage);
    }

    void diagonal_crossover()
    {
        int n = original.segment_row_length;
        crossover(n);
    }

    // really only makes sense for normal 3x3 sudokus
    void two_point_crossover()
    {
        crossover(2);
    }

    //can be called to punish sudokus that are vastly the same
    void punish_same(int punishment_strength){
        std::vector<std::vector<float>> counting = std::vector<std::vector<float>>(population[0].size, std::vector<float>(population[0].row_length, 0));
        for(int i = 0; i < population_size; i++){
            for(int ii = 0; ii < population[i].size; ii++){
                counting[ii][*(population[i].row_representation[ii])-1]+=1/(float)population_size;
            }
        }
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

    //helper struct to easily sort sudokus
    struct item
    {
        float fitness_sum;
        Sudoku<int> population_element;
        Sudoku<float> fitness_element;
    };

    // selection base on fitness 
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
        for (int i = 0; i < new_population.size(); i++)
        {
            population.push_back(new_population[i].population_element);
            fitness_sudokus.push_back(new_population[i].fitness_element);
        }
        population_size = population.size();
    }

    //takes the worst (must be fixed)
    void stochastic_universal_sampling(float keeping_percentage){
        assert(keeping_percentage <= 100);

        //min-max normalization of fitness_sums
        float sum = 0;
        float max = *std::max_element(fitness_sums.begin(), fitness_sums.end());
        float min = *std::min_element(fitness_sums.begin(), fitness_sums.end());
        for (int i = 0; i < fitness_sums.size(); i++)
        {
            fitness_sums[i] = (fitness_sums[i] - min) / (max - min);
            sum += fitness_sums[i];
        }

        //prepare roulette wheel
        std::vector<float> roulette_wheel = std::vector<float>(0); 
        float pointer = 0;
        for(int i = 0; i < fitness_sums.size(); i++){
            roulette_wheel.push_back(1 - fitness_sums[i] / sum + pointer);  //(1-) so that worst is smallest
            pointer += 1 - fitness_sums[i] / sum;
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
        for (int i = 0; i < new_population.size(); i++)
        {
            population.push_back(new_population[i].population_element);
            fitness_sudokus.push_back(new_population[i].fitness_element);
        }
        population_size = population.size();
    }
    // resets the list to a ordered list of all elements that can be set
    std::vector<float> reset_list(int size)
    {
        std::vector<float> list = {};
        for (int i = 0; i < size; i++)
        {
            list.push_back(i + 1);
        }
        return list;
    }

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

    //initialization also takes row collision as a factor
    // which pair of collision is choose randomly between checkrow && (checkcoloumn || checkgrid)
    //1. random start position in line where i start to add 
    void initialize_smart(){
        // create random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 1);
        for (auto it = population.begin(); it != population.end(); it++){ //iterate over population
            bool horizontal = dist(gen);
            std::vector<std::vector<std::shared_ptr<int>>> grid = it->grid_representation;
            for (int i = 0; i < grid.size(); i++){ //iterate over subgrids
                std::vector<std::vector<int>> possibles = std::vector<std::vector<int>>(it->segment_row_length, std::vector<int>(0));
                std::vector<std::vector<int>> how_common = std::vector<std::vector<int>>(it->segment_row_length,std::vector<int>(it->row_length, 0));
                //find possibles for each row of the grid
                for (int ii = 0; ii < possibles.size(); ii++){ //iterate over rows of subgrid
                    for(int iii = 0; iii < it->row_length; iii++){ //possible elements for the row
                        if(checkrow(*it, ii+it->segment_row_length*(i/it->segment_row_length), iii+1) && horizontal && checkgrid(*it, i, iii+1)){
                            possibles[ii].push_back(iii+1);
                            for(int iv = 0; iv < it->segment_row_length; iv++){
                                if(iv != ii) how_common[iv][iii]++;
                            }
                        }
                        else if (checkcoloumn(*it, ii+(i%it->segment_row_length)*it->segment_row_length, iii+1) && !horizontal && checkgrid(*it, i, iii+1)){
                            possibles[ii].push_back(iii+1);
                            for(int iv = 0; iv < it->segment_row_length; iv++){
                                if(iv != ii) how_common[iv][iii]++;
                            }
                        }
                    }
                }
                //sort by possibles by how common
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
                for (int ii = 0; ii < grid[i].size(); ii++){
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
                    if(horizontal){
                        while(*(grid[i][pos*it->segment_row_length+((first[pos]+insertstart)%it->segment_row_length)]) != 0){
                            first[pos]++;
                            if(first[pos] == it->segment_row_length){
                                possibles[pos] = std::vector<int>(0);
                                break;
                            }
                        }
                        if(first[pos] == it->segment_row_length) continue;
                        //put into first pos
                        *(grid[i][pos*it->segment_row_length+((first[pos]+insertstart)%it->segment_row_length)]) = possibles[pos][0];
                        //erase used number from all possibles
                        for(int iii = 0; iii < possibles.size(); iii++){
                            possibles[iii].erase(std::remove(possibles[iii].begin(), possibles[iii].end(), *(grid[i][pos*it->segment_row_length+((first[pos]+insertstart)%it->segment_row_length)])), possibles[iii].end());
                        }
                    }
                    else{
                        while(*(grid[i][((first[pos]+insertstart)%it->segment_row_length)*it->segment_row_length+pos]) != 0){
                            first[pos]++;
                            if(first[pos] == it->segment_row_length){
                                possibles[pos] = std::vector<int>(0);
                                break;
                            }
                        }
                        if(first[pos] == it->segment_row_length) continue;
                        //put into first pos
                        *(grid[i][((first[pos]+insertstart)%it->segment_row_length)*it->segment_row_length+pos]) = possibles[pos][0];
                        //erase used number from all possibles 
                        for(int iii = 0; iii < possibles.size(); iii++){
                            possibles[iii].erase(std::remove(possibles[iii].begin(), possibles[iii].end(), *(grid[i][((first[pos]+insertstart)%it->segment_row_length)*it->segment_row_length+pos])), possibles[iii].end());
                        }
                    }
                }
                //fill leftovers (collides with row, but is rare)
                for(int ii = 0; ii < grid.size();ii++){
                    if(*(grid[i][ii]) == 0){
                        for(int iii = 1; iii < 10; iii++){
                            if(checkgrid(*it, i, iii)){
                                *(grid[i][ii]) = iii;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    void calculate_all_fitness(){
        for (int i = 0; i < population.size(); i++)
        {
            fitness_sudokus.push_back(Sudoku<float>(population[i].row_representation.size()));
        }
        for (int i = 0; i < population.size(); i++){
            float sum = 0;
            // g_i1(x)=|45-sum_{j=1}^9(x_{i,j})| = row sum
            // g_j2(x)=|45-sum_{j=1}^9(x_{j,i})| = coloumn sum
            // g_i2(x)=|9!-product_{j=1}^9(x_{i,j})| = coloumn product
            // g_j2(x)=|9!-product_{j=1}^9(x_{j,i})| = row product
            // g_i3(x)=|{1-9}\setminus{x_{i,j}}| = rowmissing
            // g_j3(x)=|{1-9}\setminus{x_{j,i}}| = coloumnmissing
            //fitness = 10*(g_i1(x)+g_i1(x))+sqrt(g_i2(x))+sqrt(g_j2(x))+50*(g_i3(x)+gj3(x))
            //as proposed in https://www.researchgate.net/profile/Kim-Viljanen/publication/228840763_New_Developments_in_Artificial_Intelligence_and_the_Semantic_Web/links/09e4150a2d2cbb80ff000000/New-Developments-in-Artificial-Intelligence-and-the-Semantic-Web.pdf#page=91
            std::vector<float> rowsum = std::vector<float>(population[i].row_length, 0);
            std::vector<float> coloumnsum = std::vector<float>(population[i].row_length, 0);
            std::vector<float> rowproduct = std::vector<float>(population[i].row_length, 1);
            std::vector<float> coloumnproduct = std::vector<float>(population[i].row_length, 1);
            std::vector<std::vector<float>> rowmissing = std::vector<std::vector<float>>(population[i].row_length, reset_list(population[i].row_length));
            std::vector<std::vector<float>> coloumnmissing = std::vector<std::vector<float>>(population[i].row_length, reset_list(population[i].row_length));
            std::vector<float> rowmissingvalues = std::vector<float>(population[i].row_length, 0);
            std::vector<float> coloumnmissingvalues = std::vector<float>(population[i].row_length, 0);
            for(int ii = 0; ii < population[i].row_representation.size(); ii++){
                int row = ii/population[i].row_length;
                int coloumn = ii%population[i].row_length;
                rowsum[row] += *(population[i].row_representation[ii]);
                coloumnsum[coloumn] += *(population[i].row_representation[ii]);
                rowproduct[row] *= *(population[i].row_representation[ii]);
                coloumnproduct[coloumn] *= *(population[i].row_representation[ii]);
                rowmissing[row].erase(std::remove(rowmissing[row].begin(), rowmissing[row].end(), *(population[i].row_representation[ii])), rowmissing[row].end());
                coloumnmissing[coloumn].erase(std::remove(coloumnmissing[coloumn].begin(), coloumnmissing[coloumn].end(), *(population[i].row_representation[ii])), coloumnmissing[coloumn].end());
            }
            for(int ii = 0; ii < population[i].row_length;ii++){
                rowsum[ii] = abs(45 - rowsum[ii]);
                coloumnsum[ii] = abs(45 - coloumnsum[ii]);
                rowproduct[ii] = abs(362880 - rowproduct[ii]);
                coloumnproduct[ii] = abs(362880 - coloumnproduct[ii]);
            }

            float minrowsum = *std::min_element(rowsum.begin(), rowsum.end());
            float maxrowsum = *std::max_element(rowsum.begin(), rowsum.end());
            float minrowproduct = *std::min_element(rowproduct.begin(), rowproduct.end());
            float maxrowproduct = *std::max_element(rowproduct.begin(), rowproduct.end());
            float minrowmissing = rowmissing[0].size();
            float maxrowmissing = rowmissing[0].size();
            for(int ii = 1; ii < population[i].row_length;ii++){
                if(rowmissing[ii].size() < minrowmissing) minrowmissing = rowmissing[ii].size();
                if(rowmissing[ii].size() > maxrowmissing) maxrowmissing = rowmissing[ii].size();
            }

            float mincoloumnsum = *std::min_element(coloumnsum.begin(), coloumnsum.end());
            float maxcoloumnsum = *std::max_element(coloumnsum.begin(), coloumnsum.end());
            float mincoloumnproduct = *std::min_element(coloumnproduct.begin(), coloumnproduct.end());
            float maxcoloumnproduct = *std::max_element(coloumnproduct.begin(), coloumnproduct.end());
            float mincoloumnmissing = coloumnmissing[0].size();
            float maxcoloumnmissing = coloumnmissing[0].size();
            for(int ii = 1; ii < population[i].row_length;ii++){
                if(coloumnmissing[ii].size() < mincoloumnmissing) mincoloumnmissing = coloumnmissing[ii].size();
                if(coloumnmissing[ii].size() > maxcoloumnmissing) maxcoloumnmissing = coloumnmissing[ii].size();
            }

            for(int ii = 0; ii < population[i].row_length;ii++){
                rowsum[ii] = (rowsum[ii] - minrowsum) / (maxrowsum - minrowsum);
                rowproduct[ii] = (rowproduct[ii] - minrowproduct) / (maxrowproduct - minrowproduct);
                rowmissingvalues[ii] = (rowmissing[ii].size() - minrowmissing) / (maxrowmissing - minrowmissing);

                coloumnsum[ii] = (coloumnsum[ii] - mincoloumnsum) / (maxcoloumnsum - mincoloumnsum);
                coloumnproduct[ii] = (coloumnproduct[ii] - mincoloumnproduct) / (maxcoloumnproduct - mincoloumnproduct);
                coloumnmissingvalues[ii] = (coloumnmissing[ii].size() - mincoloumnmissing) / (maxcoloumnmissing - mincoloumnmissing);
            }

            for(int ii = 0; ii < population[i].row_representation.size(); ii++){
                int row = ii/population[i].row_length;
                int coloumn = ii%population[i].row_length;
                float tmp = 0;
                tmp += rowsum[row] + coloumnsum[coloumn] + rowproduct[row] + coloumnproduct[coloumn] + rowmissingvalues[row] + coloumnmissingvalues[coloumn];
                *fitness_sudokus[i].row_representation[ii] += tmp;
                sum += tmp;
            }
            fitness_sums.push_back(sum);
            //calculate fitness sums of grids
            std::vector<float> gridsum = std::vector<float>(0);
            for(int ii = 0; ii < population[i].row_length;ii++){ //iterate over grids
                sum = 0;
                for(int iii = 0; iii < population[i].row_length; iii++){ //iterate over elements of grids
                    sum += *(fitness_sudokus[i].grid_representation[ii][iii]);
                }
                gridsum.push_back(sum);
            }
        }
    }

    void calculate_collision_fitness()
    {
        fitness_sums.clear();
        fitness_sudokus = std::vector<Sudoku<float>>(0);
        for (int i = 0; i < population.size(); i++)
        {
            fitness_sudokus.push_back(Sudoku<float>(population[i].size));
        }
        for (int i = 0; i < population.size(); i++){
            int sum = 0;
            //calcualte collision with others
            std::vector<std::vector<std::shared_ptr<float>>> hashmap;
            // rows
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
                        *hashmap[iii][iv] += (hashmap[iii].size()) > 1;
                        sum += (hashmap[iii].size() > 1);
                    }
                }
            }
            fitness_sums.push_back(sum);
        }
    }

    // crossovers are done at the horitzontal borders or grid rows
    void crossover(int n)
    {
        assert(population_size % n == 0);
        int copy_element_size = (population[0].row_length) * (population[0].segment_row_length);
        std::vector<Sudoku<int>> children = std::vector<Sudoku<int>>(population_size);
        std::vector<Sudoku<float>> fitness_children = std::vector<Sudoku<float>>(population_size);
        std::vector<Sudoku<int>> mutated = {};
        for (int i = 0; i < population_size; i += n)
        { // iterate over population
            for (int ii = 0; ii < n; ii++)
            { // initailize children by copying parents
                children[i + ii] = Sudoku<int>(population[i + ii]);
                fitness_children[i + ii] = Sudoku<float>(fitness_sudokus[i + ii]);
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
                        *(fitness_children[i + ii].row_representation[(ii + parent) % n * copy_element_size + iii]) 
                        = *(fitness_sudokus[i + parent].row_representation[(ii + parent) % n * copy_element_size + iii]);
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
            fitness_sudokus.push_back(fitness_children[i]);
            fitness_sums.push_back(0);
        }
        population_size = population.size();
    }

    bool checkrow(Sudoku<int> sudoku, int row, int number){
        for(int i = 0; i < sudoku.row_length; i++){
            if(*(sudoku.row_representation[row*sudoku.row_length+i]) == number) return false;
        }
        return true; 
    }

    bool checkcoloumn(Sudoku<int> sudoku, int coloumn, int number){
        for(int i = 0; i < sudoku.row_length; i++){
            if(*(sudoku.row_representation[i*sudoku.row_length+coloumn]) == number) return false;
        }
        return true; 
    }

    bool checkgrid(Sudoku<int> sudoku, int grid, int number){
        for(int i = 0; i < sudoku.row_length; i++){
            if(*(sudoku.grid_representation[grid][i]) == number) return false;
        }
        return true; 
    }

    //pick a random element and only switch it if the position really works
    /*void strict_mutation(int mutation_rate){
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 100);
        for(int  i = 0; i < population_size;i++){ //iterate over population
            for(auto it = population[i].get_grid_representaion().begin(); it != population[i].get_grid_representaion().end()){ //iterate over grids
                for(auto itt = it->begin(); itt != it->end(); itt++){ //iterate over elements of grids
                    if(*(original.grid_representation[i][ii]) != 0) continue;
                    if(){ //fitness are values between 0 and 6 so *16,6 == between 0 and 100
                        *(itt) = dist(rd);
                    }
                }
            }
        }
    }*/
    // randomly choose and swap elements that have a fitness_value higher than zero
    void collision_mutation()
    {
        std::vector<Sudoku<int>> mutated = {};
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
                int rate = 100;
                if(dist(rd) < 50) rate = 0;
                for (int iii = 0; iii < population[parentpos].row_length; iii++)
                { // iterate over elements of grids
                    if (*(original.grid_representation[ii][iii]) != 0)
                    {
                        *(mutation.grid_representation[ii][iii]) = *(original.grid_representation[ii][iii]);
                    }
                    else if (dist(rd) < 10 || dist(rd) < *(fitness_sudokus[parentpos].grid_representation[ii][iii])*50) //fitness are values between 0 and 6 so *16,6 == between 0 and 100
                    {
                        swaps.push_back(iii);
                    }
                    else
                    {
                        *(mutation.grid_representation[ii][iii]) = *(population[parentpos].grid_representation[ii][iii]);
                    }
                }
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
            mutated.push_back(mutation);
        }
        population = mutated;
        fitness_sums.clear();
        fitness_sudokus = std::vector<Sudoku<float>>(0);
        population_size = population.size();
    }

    void intelligent_mutation()
    {
        std::vector<Sudoku<int>> mutated = {};
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 1);
        for (int i = 0; i < wished_population_size; i++)
        {
            int parentpos = i % population_size;
            Sudoku<int> mutation = Sudoku<int>(population[parentpos].size);
            std::vector<float> list = reset_list(population[parentpos].segment_row_length);
            std::shuffle(list.begin(), list.end(), gen);
            bool horizontal = dist(rd);
            for (int ii = 0; ii < population[i].row_length; ii += population[i].segment_row_length)
            { // iterate over gridrows
                for (int iii = 0; iii < population[i].segment_row_length; iii++)
                { // iterate over elements of gridrows
                    std::vector<int> swaps = {};
                    for (int iv = 0; iv < population[i].row_length; iv++)
                    { // iterate over elements of grids
                        if (*(original.grid_representation[ii + iii][iv]) != 0)
                        {
                            *(mutation.grid_representation[ii + iii][iv]) = *(original.grid_representation[ii + iii][iv]);
                        }
                        else if (list[ii / population[i].segment_row_length] - 1 == iii && horizontal)
                        { // choose the grid to swap for the coloumn of grids
                            if (*(fitness_sudokus[parentpos].grid_representation[ii + iii][iv]) > 0)
                            {
                                swaps.push_back(iv);
                            }
                            else
                            {
                                *(mutation.grid_representation[ii + iii][iv]) = *(population[parentpos].grid_representation[ii + iii][iv]);
                            }
                        }
                        else if (list[iii] - 1 == ii / population[i].segment_row_length && !horizontal)
                        {
                            if (*(fitness_sudokus[parentpos].grid_representation[ii + iii][iv]) > 0)
                            {
                                swaps.push_back(iv);
                            }
                            else
                            {
                                *(mutation.grid_representation[ii + iii][iv]) = *(population[parentpos].grid_representation[ii + iii][iv]);
                            }
                        }
                        else
                        {
                            *(mutation.grid_representation[ii + iii][iv]) = *(population[parentpos].grid_representation[ii + iii][iv]);
                        }
                    }
                    std::shuffle(swaps.begin(), swaps.end(), gen);
                    if (swaps.size() != 0)
                    {
                        for (int iv = 0; iv < swaps.size() - 1; iv += 2)
                        {
                            *(mutation.grid_representation[ii + iii][swaps[iv]]) = *(population[parentpos].grid_representation[ii + iii][swaps[iv + 1]]);
                            *(mutation.grid_representation[ii + iii][swaps[iv + 1]]) = *(population[parentpos].grid_representation[ii + iii][swaps[iv]]);
                        }
                        if (swaps.size() % 2 != 0)
                        {
                            *(mutation.grid_representation[ii + iii][swaps[swaps.size() - 1]]) = *(population[parentpos].grid_representation[ii + iii][swaps[swaps.size() - 1]]);
                        }
                    }
                }
            }
            mutated.push_back(mutation);
        }
        population = mutated;
        fitness_sums.clear();
        fitness_sudokus = std::vector<Sudoku<float>>(0);
        population_size = population.size();
    }
    Sudoku<int> original;
    std::vector<float> fitness_sums;
    std::vector<Sudoku<float>> fitness_sudokus; // representation of fitness values for each individual is the same
    std::vector<Sudoku<int>> population;
    int population_size;
    int wished_population_size;
};
#endif