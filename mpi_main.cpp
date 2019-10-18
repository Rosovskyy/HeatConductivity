#include <mpi.h>

#include "mpi_main.h"
#include "function.h"
#include <Magick++.h>


void swap_matrixes(Matrix &m_one, Matrix &m_two){
    Matrix tmp(m_one.getAmountOfRows(), m_two.getAmountOfCols());
    tmp.setMatr(m_one.getRowPointer(0));
    m_one.setMatr(m_two.getRowPointer(0));
    m_two.setMatr(tmp.getRowPointer(0));
}

unsigned char* temp_to_color(double min, double max, double temp) {
    double ratio = 2 * (temp - min) / (max - min);
    unsigned char r = static_cast<unsigned char>(std::max((double) 0, 255 * (ratio - 1)));
    unsigned char b = static_cast<unsigned char>(std::max((double) 0, 255 * (1 - ratio)));
    unsigned char g = 255 - b - r;
    unsigned char *arr = new unsigned char[3]{r,g,b};
    return arr;
}


void save_image(std::string name, double *arr, size_t arrsize, int width, int temp_min, int temp_max){
    unsigned char matrix[arrsize*3];
    for(auto i=0;i<arrsize;i++){
        unsigned char* pix = temp_to_color(temp_min, temp_max, arr[i]);
        matrix[3*i] = *(pix);
        matrix[3*i+1] = *(pix+1);
        matrix[3*i+2] = *(pix+2);
        delete pix;
    }
    Magick::Image img(width,arrsize/width,"RGB", Magick::CharPixel, matrix);
    img.depth(16);
    img.write(name);
}

void swap_rows(int thr_num, int world_size, Matrix &mtrx, bool value){
    double matr[mtrx.getAmountOfCols()];
    if (thr_num % 2 == 0){
        if (thr_num == 0){
            MPI_Send(mtrx.getRowCopy(mtrx.getAmountOfRows() - 2), mtrx.getAmountOfCols(), MPI_DOUBLE, thr_num+1, 0, MPI_COMM_WORLD);
            MPI_Recv(&matr, mtrx.getAmountOfCols(), MPI_DOUBLE, thr_num+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            mtrx.setRow(matr, mtrx.getAmountOfRows() - 1);
        }
        else if (thr_num == world_size - 1){
            MPI_Send(mtrx.getRowCopy( 1), mtrx.getAmountOfCols(), MPI_DOUBLE, thr_num-1, 0, MPI_COMM_WORLD);
            MPI_Recv(&matr, mtrx.getAmountOfCols(), MPI_DOUBLE, thr_num-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            mtrx.setRow(matr, 0);
        }else{
            MPI_Send(mtrx.getRowCopy(1), mtrx.getAmountOfCols(), MPI_DOUBLE, thr_num-1, 0, MPI_COMM_WORLD);
            MPI_Recv(&matr, mtrx.getAmountOfCols(), MPI_DOUBLE, thr_num-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            mtrx.setRow(matr, 0);

            MPI_Send(mtrx.getRowCopy(mtrx.getAmountOfRows() - 1), mtrx.getAmountOfCols(), MPI_DOUBLE, thr_num+1, 0, MPI_COMM_WORLD);
            MPI_Recv(&matr, mtrx.getAmountOfCols(), MPI_DOUBLE, thr_num+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            mtrx.setRow(matr, mtrx.getAmountOfRows() - 1);
        }
    }
    else{
        if (thr_num == world_size - 1) {
            MPI_Recv(&matr, mtrx.getAmountOfCols(), MPI_DOUBLE, thr_num-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(mtrx.getRowCopy(1), mtrx.getAmountOfCols(), MPI_DOUBLE, thr_num-1, 0, MPI_COMM_WORLD);
            mtrx.setRow(matr, 0);
        }
        else{
            MPI_Recv(&matr, mtrx.getAmountOfCols(), MPI_DOUBLE, thr_num-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(mtrx.getRowCopy(1), mtrx.getAmountOfCols(), MPI_DOUBLE, thr_num-1, 0, MPI_COMM_WORLD);
            mtrx.setRow(matr, 0);

            MPI_Recv(&matr, mtrx.getAmountOfCols(), MPI_DOUBLE, thr_num+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(mtrx.getRowCopy(mtrx.getAmountOfRows() - 1), mtrx.getAmountOfCols(), MPI_DOUBLE, thr_num+1, 0, MPI_COMM_WORLD);
            mtrx.setRow(matr, mtrx.getAmountOfRows() - 1);
        }
    }

}

void spread(Matrix &mtrx, configs_strct &configs) {

    MPI_Init(nullptr, nullptr);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size < 2) {
        std::cerr << "Please select more processes";
        exit(1);
    }
    world_size -= 1;
    int rows_per_thread = (mtrx.getAmountOfRows() - 2) / world_size, threads_left =
            (mtrx.getAmountOfRows() - 2) % world_size;
    int thr_num;
    MPI_Comm_rank(MPI_COMM_WORLD, &thr_num);
    int m = 0;


    int my_row_start = rows_per_thread * thr_num, my_row_finish = my_row_start + rows_per_thread + 1;
    if (thr_num == world_size - 1)
        my_row_finish += threads_left;

    Matrix matr_one(my_row_finish - my_row_start + 1, mtrx.getAmountOfCols());
    Matrix matr_two(my_row_finish - my_row_start + 1, mtrx.getAmountOfCols());
    double *ar = mtrx.getRowPointer(my_row_start);
    matr_one.setMatr(ar);
    matr_two.setCopyMatr(ar, (my_row_finish - my_row_start + 1) * mtrx.getAmountOfCols());

    for (;;) {
        m++;
        if (m == 1000) {
            break;
        }
        if (thr_num != world_size) {
            for (int row = 1; row < matr_one.getAmountOfRows() - 1; ++row) {
                for (int col = 1; col < matr_one.getAmountOfCols() - 1; ++col) {
                    double newVal = func(matr_one, row, col, configs);
                    matr_two.setEl(newVal, row, col);
                }
            }

            swap_matrixes(matr_one, matr_two);
            if (world_size > 1) {
                swap_rows(thr_num, world_size, matr_one, false);
                swap_rows(thr_num, world_size, matr_two, true);
            }

            double *array = nullptr;

            if (thr_num == 0) {
                array = matr_one.getRowCopy(matr_one.getAmountOfRows() - 1);
                matr_two.setRow(array, matr_two.getAmountOfRows() - 1);
            } else if (thr_num == world_size) {
                array = matr_one.getRowCopy(0);
                matr_two.setRow(array, 0);
            } else {
                array = matr_one.getRowCopy(0);
                matr_two.setRow(array, 0);

                array = matr_one.getRowCopy(matr_one.getAmountOfRows() - 1);
                matr_two.setRow(array, matr_two.getAmountOfRows() - 1);
            }


            if (thr_num == 0) {
                auto *mtr = matr_one.rowsToArray(0, matr_one.getAmountOfRows() - 1);
                MPI_Send(mtr, matr_one.getAmountOfCols() * (matr_one.getAmountOfRows() - 1), MPI_DOUBLE, world_size, 0,
                         MPI_COMM_WORLD);
            } else if (thr_num == world_size - 1) {
                MPI_Send(matr_one.rowsToArray(1, matr_one.getAmountOfRows()),
                         matr_one.getAmountOfCols() * (matr_one.getAmountOfRows() - 1), MPI_DOUBLE, world_size, 0,
                         MPI_COMM_WORLD);
            } else {
                MPI_Send(matr_one.rowsToArray(1, matr_one.getAmountOfRows() - 1),
                         matr_one.getAmountOfCols() * (matr_one.getAmountOfRows() - 2), MPI_DOUBLE, world_size, 0,
                         MPI_COMM_WORLD);
            }
        } else {
            Matrix mkrx(configs.width, configs.height);
            int num = 0;
            int n = 0;
            for (auto i = 0; i < world_size; i++) {
                if (i == 0) {
                    num = rows_per_thread + 1;
                } else if (i == world_size - 1) {
                    num = rows_per_thread + threads_left + 1;
                } else {
                    num = rows_per_thread;
                }

                double test_matr[mtrx.getAmountOfCols() * num];
                //            double* buf[mkrx.getAmountOfCols()*(num)];
                MPI_Recv(&test_matr, mkrx.getAmountOfCols() * num, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                for (int j = 0; j < num; ++j) {
                    mkrx.setRow(test_matr + j * mkrx.getAmountOfCols(), j + n);
                }
                n += num;

                std::cout << "\n";
            }
            std::string imageName = "../imagesDoNotDelete/image" + std::to_string(m) + ".bmp";
            save_image(imageName,mkrx.retAr(),mkrx.getAmountOfCols()*mkrx.getAmountOfRows(), 100, 0, 100);
        }


    }
    MPI_Finalize();
}