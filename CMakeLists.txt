cmake_minimum_required(VERSION 3.15)
project(NewHeatConductivity)

set(CMAKE_CXX_STANDARD 17)

find_package(MPI REQUIRED)

SET(CMAKE_C_COMPILER "/usr/bin/mpicc")
SET(CMAKE_CXX_COMPILER "/usr/bin/mpic++")

include_directories(${MPI_INCLUDE_PATH})

#add_executable(HeatConductivity test.cpp)
add_executable(NewHeatConductivity main.cpp matrix.h read_configs.cpp read_configs.h function.h mpi_main.cpp mpi_main.h)
#add_executable(NewHeatConductivity random.cpp)
find_package(ImageMagick COMPONENTS Magick++)
include_directories(${ImageMagick_INCLUDE_DIRS})
target_link_libraries(NewHeatConductivity ${ImageMagick_LIBRARIES})

#-np 3 ./NewHeatConductivity