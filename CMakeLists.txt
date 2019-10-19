cmake_minimum_required(VERSION 3.15)
project(NewHeatConductivity)

set(CMAKE_CXX_STANDARD 17)

find_package(MPI REQUIRED)

SET(CMAKE_C_COMPILER "/usr/bin/mpicc")
SET(CMAKE_CXX_COMPILER "/usr/bin/mpic++")

include_directories(${MPI_INCLUDE_PATH})
include_directories(headers)

add_executable(NewHeatConductivity sources/main.cpp headers/matrix.h sources/read_configs.cpp headers/read_configs.h headers/function.h sources/mpi_main.cpp headers/mpi_main.h)
find_package(ImageMagick COMPONENTS Magick++)
include_directories(${ImageMagick_INCLUDE_DIRS})
target_link_libraries(NewHeatConductivity ${ImageMagick_LIBRARIES})
