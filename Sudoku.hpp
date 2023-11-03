#ifndef SUDOKU_H_
#define SUDOKU_H_
#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<math.h>
#include<random>
#include<algorithm>

//class that takes a row representation and save it and the grid representation
class Sudoku{
    private:
    int row_length;
    int segment_row_length;
    std::vector<int*> row_representation; //representations containing the unsolved sudoku
    std::vector<std::vector<int*>> grid_reprensentation;

    //transforms the row representation to the grid representation
    void transform_to_grid(std::vector<int*> input){
        grid_reprensentation = std::vector<std::vector<int*>>(row_length,std::vector<int*>(0));
        for(int i = 0; i < input.size()/row_length; i++){ //iterating over the rows
            for(int ii = 0; ii < row_length/segment_row_length; ii++){ //iterating over the segments in the rows 
                for(int iii = 0; iii < segment_row_length; iii++){ //iterating over the elements of the segments
                    grid_reprensentation[ii+floor(i/segment_row_length)*segment_row_length].push_back(input[i*row_length+ii*segment_row_length+iii]);
                }
            }
        }
    }

    public:
    //default Constructor
    Sudoku(){
        row_length = 0;
        segment_row_length = 0;
        row_representation = std::vector<int*>(0);
        grid_reprensentation = std::vector<std::vector<int*>>(0,std::vector<int*>(0));
    }

    //Constructor transforming the row representation to the grid representation and saving everything
    Sudoku(std::vector<int*> row_representation){
        this->row_representation = row_representation;
        this->row_length = sqrt(row_representation.size());
        this->segment_row_length = sqrt(row_length);
        transform_to_grid(row_representation);
    }

    //copy by value
    Sudoku get_copy(){
        std::vector<int*> row_rep;
        for(auto it = row_representation.begin(); it != row_representation.end(); it++){
            row_rep.push_back(new int(**it));
        }
        return Sudoku(row_rep);
    }

    //getters
    std::vector<int*> get_row_representation(){
        return row_representation;
    }
    std::vector<std::vector<int*>> get_grid_representation(){
        return grid_reprensentation;
    }

    //print
    void print(){
        std::string horizontal_line = std::string(row_length*2+segment_row_length,'-');
        for(int i = 0; i < row_representation.size(); i++){
            if(i%segment_row_length==0 && i%row_length!=0) std::cout << "| ";
            if(i%row_length==0 && i!=0) std::cout << std::endl;
            if(i%(row_length*segment_row_length)==0 && i!=0) std::cout << horizontal_line << std::endl;
            std::cout << *(row_representation[i]) << " ";
        }
        std::cout << "\n\n";
    }
};
#endif