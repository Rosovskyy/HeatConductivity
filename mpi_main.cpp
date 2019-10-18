#include <mpi.h>

#include "mpi_main.h"
#include "function.h"


void swap_matrixes(Matrix &m_one, Matrix &m_two){
    Matrix tmp(m_one.getAmountOfRows(), m_two.getAmountOfCols());
    tmp.setMatr(m_one.getRowPointer(0));
    m_one.setMatr(m_two.getRowPointer(0));
    m_two.setMatr(tmp.getRowPointer(0));
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

    if (world_size < 2){
        std::cerr << "Please select more processes";
        exit(1);
    }
    world_size -= 1;
    int rows_per_thread = (mtrx.getAmountOfRows() - 2) / world_size, threads_left = (mtrx.getAmountOfRows() -2) % world_size;
    int thr_num;
    MPI_Comm_rank(MPI_COMM_WORLD, &thr_num);
    int m = 0;

    std::cout << "world size: " << world_size << std::endl;

    int my_row_start = rows_per_thread * thr_num, my_row_finish = my_row_start + rows_per_thread + 1;
    if (thr_num == world_size - 1)
        my_row_finish += threads_left;

    std::cout << "thr_num: " << thr_num << ", start: " << my_row_start << ", finish: " << my_row_finish << std::endl;

    double matr[mtrx.getAmountOfCols()];

    if (thr_num != world_size) {
        Matrix matr_one(my_row_finish - my_row_start + 1, mtrx.getAmountOfCols());
        Matrix matr_two(my_row_finish - my_row_start + 1, mtrx.getAmountOfCols());
        double *ar = mtrx.getRowPointer(my_row_start);
        matr_one.setMatr(ar);
        matr_two.setCopyMatr(ar, (my_row_finish - my_row_start + 1) * mtrx.getAmountOfCols());

        for (;;) {
            m++;
            if (m == 10) {
                break;
            }
            for (int row = 1; row < matr_one.getAmountOfRows() - 1; ++row) {
                for (int col = 1; col < matr_one.getAmountOfCols() - 1; ++col) {
                    double newVal = func(matr_one, row, col, configs);
                    matr_two.setEl(newVal, row, col);
                }
            }
//            if (thr_num == 2){
//                for (int i=0; i < matr_two.getAmountOfRows(); ++i){
//                    for (int j=0; j < matr_two.getAmountOfCols(); ++j){
//                        std::cout << matr_two.getEl(i,j) << " ";
//                    }
//                    std::cout << "     66 \n";
//                }
//                std::cout << "\n";
//            }
            swap_matrixes(matr_one, matr_two);
            if (world_size > 1){
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

//            if (thr_num == 2){
//                for (int i=0; i < matr_one.getAmountOfRows(); ++i){
//                    for (int j=0; j < matr_one.getAmountOfCols(); ++j){
//                        std::cout << matr_one.getEl(i,j) << " ";
//                    }
//                    std::cout << "     99 \n";
//                }
//                std::cout << "\n";
//            }

            if (thr_num == 0) {
                MPI_Send(matr_one.rowsToArray(0,matr_one.getAmountOfCols()-1), matr_one.getAmountOfCols()-1, MPI_DOUBLE, world_size, 0 ,MPI_COMM_WORLD);
            }
            else if (thr_num == world_size-2) {
                MPI_Send(matr_one.rowsToArray(1,matr_one.getAmountOfCols()), matr_one.getAmountOfCols()-1, MPI_DOUBLE, world_size, 0 ,MPI_COMM_WORLD);
            }
            else {
                MPI_Send(matr_one.rowsToArray(1,matr_one.getAmountOfCols()-1), matr_one.getAmountOfCols()-2, MPI_DOUBLE, world_size, 0 ,MPI_COMM_WORLD);
            }
        }
    }
    else {
        Matrix mkrx(configs.width,configs.height);
        int num = 0;
        for(auto i=0; i<world_size-1; i++){
            if (i != world_size-2){
                num = rows_per_thread;
            }
            else{
                num = rows_per_thread+threads_left;
            }
            double test_matr[mtrx.getAmountOfCols()*num];
            double* buf[mkrx.getAmountOfCols()*(num)];
            MPI_Recv(&test_matr, mkrx.getAmountOfCols(), MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int n = 0;
//            for (auto k: test_matr) {
//                std::cout << k << std::endl;
//            }
//            for(auto j=0;j < mkrx.getAmountOfCols()*(num);j++){
//                mkrx.setRow(test_matr+n, i);
//                n+=mkrx.getAmountOfCols();
//            }
        }
        mkrx.printContent();
    }

//    if (thr_num == 0) {
//        mtrx.printContent();
//        std::cout << "\n";
//    }
//    MPI_Barrier(MPI_COMM_WORLD);
//    if (thr_num == 1) {
//        mtrx.printContent();
//        std::cout << "\n";
//    }
//    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Finalize();
}