#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

#include "../headers/read_configs.h"
void read_constants(configs_strct *configs, const char * filename){
    std::ifstream file(filename, std::ios_base::out);
    if (!file.is_open())
        throw FileOpeningException();

    std::string line;
    std::getline(file,line);
    configs->density = std::stod(line.substr(line.find_first_of("=")+1));
    std::getline(file,line);
    configs->cap = std::stod(line.substr(line.find_first_of("=")+1));
    std::getline(file,line);
    configs->conduct = std::stod(line.substr(line.find_first_of("=")+1));
    std::getline(file,line);
    configs->width = std::stoi(line.substr(line.find_first_of("=")+1));
    std::getline(file,line);
    configs->height = std::stoi(line.substr(line.find_first_of("=")+1));
    std::getline(file,line);
    configs->delta_x = std::stod(line.substr(line.find_first_of("=")+1));
    std::getline(file,line);
    configs->delta_y = std::stod(line.substr(line.find_first_of("=")+1));
    std::getline(file,line);
    configs->delta_t = std::stod(line.substr(line.find_first_of("=")+1));
    std::getline(file,line);
    configs->save_t = std::stod(line.substr(line.find_first_of("=")+1));
}

void read_matrix(Matrix &mtrx, const char * filename ) {
    std::fstream file(filename, std::fstream::in);
    std::vector<std::vector<std::string> > data;

    if (file.is_open()) {
        int row = 0, col;

        while (file) {
            col = 0;
            std::string str_row;
            if (!getline(file, str_row)) break;

            std::istringstream ss(str_row);

            std::string s;
            while (ss) {
                if (!getline(ss, s, '|')) break;
                mtrx.setEl(stoi(s), row, col);
                col++;
            }
            row++;
        }
    } else {
        throw FileOpeningException();
    }
}
