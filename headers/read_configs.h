
#ifndef HEATCONDUCT_READ_CONFIGS_H
#define HEATCONDUCT_READ_CONFIGS_H

#include "matrix.h"
struct FileOpeningException : public std::exception {
    const char * what () const throw () {
//        std::cout << "Couln't open file";
        return "Couln't open file";
    }
};
typedef struct structure{
    double density;
    double cap;
    double conduct;
    int width;
    int height;
    double delta_x;
    double delta_y;
    double delta_t;
    int save_t;
    double lambda;
} configs_strct;


void read_constants(configs_strct *configs, const char * filename);
void read_matrix(Matrix &mtrx, const char * filename);
#endif //HEATCONDUCT_READ_CONFIGS_H
