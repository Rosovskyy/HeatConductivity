#include <iostream>
#include <cmath>

#include "matrix.h"
#include "read_configs.h"
#include "mpi_main.h"
int main(int argc, char *argv[]) {
    configs_strct configs;
    read_constants(&configs, "../txt/configs.txt");

    configs.lambda = configs.conduct / (configs.density * configs.cap);
    if (configs.delta_t > (std::pow(std::max(configs.delta_x, configs.delta_y),2) / (4 * configs.lambda))){
        std::cerr << "Von Neumann criteria is not fulfilled";
        return 1;
    }

    Matrix mtrx(configs.width,configs.height);

    read_matrix(mtrx, "../txt/matrix2.txt");

//    std::cout << mtrx.getAmountOfRows() << " " << mtrx.getAmountOfCols();
    spread(mtrx, configs);

    return 0;
}