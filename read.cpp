#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<math.h>
#include<random>
#include<algorithm>


//Representation is in the 9x9 subgrids therefore the original sudoku has to be transformed
std::vector<std::vector<int*>> transform_to_grid(std::vector<int*> input){
    int row_length = sqrt(input.size());
    int segment_row_length = sqrt(row_length);
    std::vector<std::vector<int*>> tmpgrid = std::vector<std::vector<int*>>(row_length,std::vector<int*>(0));
    for(int i = 0; i < input.size()/row_length; i++){ //iterating over the rows
        for(int ii = 0; ii < row_length/segment_row_length; ii++){ //iterating over the segments in the rows 
            for(int iii = 0; iii < segment_row_length; iii++){ //iterating over the elements of the segments
                tmpgrid[ii+floor(i/segment_row_length)*segment_row_length].push_back(input[i*row_length+ii*segment_row_length+iii]);
            }
        }
    }
    return tmpgrid;
}

//read in the sudoku from a given path
std::vector<std::vector<int*>> readfiles(std::vector<std::string> input_paths, int number){
    std::fstream fstream;
    std::string tp;
    std::vector<std::vector<int*>> sudokus;
    
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
            sudokus.push_back(tmp);
            howmany++;
            if(howmany > number) break;
        }
        fstream.close();
    }
    return sudokus;
}

std::vector<int> reset_list(int size){
    std::vector<int> list = {};
    for(int i = 0; i < size; i++){
        list.push_back(i+1);
    }
    return list;
}

std::vector<std::vector<int*>> initial(std::vector<std::vector<int*>> sudoku){
    //create random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    for(int i = 0; i < sudoku.size(); i++){
        std::vector<int> list = reset_list(sudoku.size());
        std::shuffle(list.begin(),list.end(),gen);
        for(int ii = 0; ii < sudoku[i].size(); ii++){
            if(*(sudoku[i][ii])==0) continue;
            auto it = std::find(list.begin(),list.end(),*(sudoku[i][ii]));
            int tmp = (*it);
            (*it) = list[ii];
            list[ii] = tmp;
        }
        for(int ii = 0; ii < sudoku[i].size(); ii++){
            delete sudoku[i][ii];
            sudoku[i][ii] = new int(list[ii]);
        }
    }
    return sudoku;
}

/*
std::pair<int,std::vector<std::vector<int>>> fitness(std::vector<std::vector<int>> sudoku){
    std::vector<std::vector<int>> collisions = std::vector<std::vector<int>>(sudoku.size(),std::vector<int>(sudoku.size(),0));
    int fitness = 0;
    std::vector<std::vector<*int>> hashmap;
    //as no collisions in the grids are possible, we have to check rows and columns
    int grids_in_row = sqrt(sudoku.size()); // = numbers_in_grid_row
    for(int i = 0; i < sudoku.size(); i+=grids_in_row){ //iterate over grid rows
        for(int iii = 0; iii < sudoku.size(); iii+=grids_in_row){ //iterate over rows of grid
            hashmap = std::vector<std::vector<*int>>(sudoku.size(),std::vector<*int>(0));
            //fill hashmap
            for(int ii = 0; ii < grids_in_row; ii++){ //iterate over grids
                for(int iv = 0; iv < grids_in_row; iv++){ //iterate over elements in row of grid
                    hashmap[sudoku[i+ii][iii+iv]-1].push_back(*collisions[i+ii][iii+iv]);
                }
            }
            //add hashmap
            for(int j = 0; j < hashmap.size(); j++){
                for(int jj = 0; jj < hashmap[j].size(); jj++){
                    (*hashmap[j][jj])+=hashmap[j].size();
                    fitness+=hashmap[j].size();
                }
            }
        }
    }
    return std::make_pair(fitness, collisions);
}
*/
int main(){
    std::vector<std::string> input_paths= {"data/puzzles0_kaggle"};//,"data/puzzles4_forum_hardest_1905"};
    std::vector<std::vector<int*>> sudokus = readfiles(input_paths, 1);
    std::vector<std::vector<int*>> transformed = transform_to_grid(sudokus[0]);
    for(int i = 0; i < transformed.size(); i++){
        std::cout << " ";
        for(int ii = 0; ii < transformed[i].size(); ii++){
            std::cout << *(transformed[i][ii]);    
        }
    }
    std::vector<std::vector<int*>> initial_gen = initial(transformed);
    std::cout << "\n\n";
    return 1;
}