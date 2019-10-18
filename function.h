#ifndef HEATCONDUCTIVITY_FUNCTION_H
#define HEATCONDUCTIVITY_FUNCTION_H
#include "matrix.h"
#include "read_configs.h"

double func(Matrix &matrix, int row, int col, configs_strct &strct){
    double dx = strct.delta_x, dy = strct.delta_y, dt = strct.delta_t, lambda = strct.lambda;
    double X = (matrix.getEl(row-1, col) - 2 * matrix.getEl(row,col) + matrix.getEl(row+1,col)) / (dx * dx);
    double Y = (matrix.getEl(row, col-1) - 2 * matrix.getEl(row,col) + matrix.getEl(row,col+1)) / (dy * dy);
    return matrix.getEl(row, col)+dt*lambda*(X+Y);
}

#endif //HEATCONDUCTIVITY_FUNCTION_H
