#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<math.h>
#include<random>
#include<algorithm>

//class that takes a row representation and save it and the grid representation
class Sudoku{
    public:
    //Constructor transforming the row representation to the grid representation and saving everything
    Sudoku(std::vector<std::vector<int*>> row_representation){
        this->row_representation = row_representation;
        this->row_length = row_representation.size();
        this->segment_row_length = sqrt(row_length);
        this->grid_reprensentation = transform_to_grid(row_representation);
    }

    //copy by value
    Sudoku get_copy(){
        std::vector<std::vector<int*>> row_rep;
        for(auto i = row_representation.begin(); i != row_representation.end(); i++){
            row_rep.push_back(new int(**i));
        }
        return Sudoku(row_representation);
    }

    private:
    int row_length;
    int segment_row_length;
    std::vector<std::vector<int*>> row_representation; //representations containing the unsolved sudoku
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
}
