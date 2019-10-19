
#ifndef HEATCONDUCTIVITY_MPI_MAIN_H
#define HEATCONDUCTIVITY_MPI_MAIN_H
#include "read_configs.h"
#include "matrix.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <functional>

void spread(Matrix &mtrx, configs_strct &configs);
#endif //HEATCONDUCTIVITY_MPI_MAIN_H
