#ifndef SUDOKU_H_
#define SUDOKU_H_
#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<math.h>
#include<random>
#include<algorithm>
#include <memory>

//class that takes a row representation and save it and the grid representation
template <typename T>
class Sudoku{
    private:

    //transforms the row representation to the grid representation
    void transform_to_grid(std::vector<std::shared_ptr<T>> input){
        grid_reprensentation = std::vector<std::vector<std::shared_ptr<T>>>(row_length,std::vector<std::shared_ptr<T>>(row_length));
        for(int i = 0; i < input.size()/row_length; i++){ //iterating over the rows
            for(int ii = 0; ii < row_length/segment_row_length; ii++){ //iterating over the segments in the rows 
                for(int iii = 0; iii < segment_row_length; iii++){ //iterating over the elements of the segments
                    grid_reprensentation[ii+floor(i/segment_row_length)*segment_row_length][iii+(i%segment_row_length)*segment_row_length]
                    = input[i*row_length+ii*segment_row_length+iii];
                }
            }
        }
    }

    public:
    //default Constructor
    Sudoku<T>(){
        size = 0;
        row_length = 0;
        segment_row_length = 0;
        row_representation = std::vector<std::shared_ptr<T>>(0);
        grid_reprensentation = std::vector<std::vector<std::shared_ptr<T>>>(0,std::vector<std::shared_ptr<T>>(0));
    }

    //construct empty sudoku with a given size
    Sudoku<T>(int size){
        this->size = size;
        row_length = sqrt(size);
        segment_row_length = sqrt(row_length);
        row_representation = std::vector<std::shared_ptr<T>>(0);
        for(int i = 0; i < size; i++){
            row_representation.push_back(std::make_shared<T>(0));
        }
        transform_to_grid(row_representation);
    }

    //Constructor transforming the row representation to the grid representation and saving everything
    Sudoku<T>(std::vector<T> row_representation){
        this->size = row_representation.size();
        this->row_representation.clear();
        for(int i = 0; i < row_representation.size(); i++){
            this->row_representation.push_back(std::make_shared<T>(row_representation[i]));
        }
        this->row_length = sqrt(row_representation.size());
        this->segment_row_length = sqrt(row_length);
        transform_to_grid(this->row_representation);
    }

    //copy constructor
    Sudoku& operator=(const Sudoku& other){
        if (this != &other) {
            // Deep copy row_representation by using the constructor above
            row_representation = std::vector<std::shared_ptr<T>>(0);
            for (const auto& element : other.row_representation) {
                if (element) {
                    row_representation.push_back(std::make_shared<T>(*element));
                } else {
                    row_representation.push_back(nullptr);
                }
            }
        }
        size = other.size;
        row_length = other.row_length;
        segment_row_length = other.segment_row_length;
        transform_to_grid(row_representation);
        return *this;
    }

    //printing the row representation with borders
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

    public:
    std::vector<std::shared_ptr<T>> row_representation; //representations containing the unsolved sudoku
    std::vector<std::vector<std::shared_ptr<T>>> grid_reprensentation;
    int size;
    int row_length;
    int segment_row_length;
};
#endif