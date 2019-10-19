#ifndef HEATCONDUCT_MATRIX_H
#define HEATCONDUCT_MATRIX_H
#include <algorithm>

class Matrix {
private:
    double *matr = nullptr;
    int cols;
    int rows;
public:
    Matrix(int rows, int cols){
        this->rows = rows;
        this->cols = cols;
        matr = new double[rows*cols];
    }
    double* getRowCopy(int row_inx){
        auto *rowAr = new double[cols];
        std::copy(&matr[row_inx * cols], &matr[row_inx * cols + cols], &rowAr[0]);
        return rowAr;
    }
    double* getRowPointer(int row_inx){
        return &matr[row_inx * cols];
    }

    void setMatr(double *mtr){
        matr = mtr;
    }
    void setCopyMatr(double *mtr, size_t size){
        std::copy(mtr, mtr + size, matr);
    }

    void setRow(const double *new_col, int row_num){
        for (int i=0; i < cols; ++i){
            matr[row_num * cols + i] = new_col[i];
        }
    }
    double getEl(int row, int col){
        return matr[row * cols + col];
    }
    void setEl(double val, int row, int col){
        matr[row * cols + col] = val;
    }
    int getAmountOfRows(){
        return rows;
    }
    int getAmountOfCols(){
        return cols;
    }
    double *retAr(){
        return matr;
    }
    void free(){
        delete matr;
        matr = nullptr;
    }
    void printContent(){
        for (int i = 0; i < rows; ++i){
            for (int j =0; j < cols; ++j){
                std::cout << getEl(i,j) << " ";
            }
            std::cout << "\n";
        }
    }

    double* rowsToArray(int f_row, int l_row){
        auto res_ar = new double[(l_row-f_row)*cols];
        for (int i=f_row*cols,j=0;i<l_row*cols; ++i,++j){
            //std::cout << matr[i] << " ";
            res_ar[j] = matr[i];
        }
        //std::cout << "\n";
        return res_ar;
    }
};


#endif //HEATCONDUCT_MATRIX_H
