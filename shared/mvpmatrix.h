#ifndef MVP_MATRIX
#define MVP_MATRIX
#include <stdbool.h>

// Размерность MVP-матрицы
#define MVP_MATRIX_DIMENSION_SIZE 4
#define MVP_MATRIX_SIZE 16

// Размерность N-матрицы
#define N_MATRIX_DIMENSION_SIZE 3
#define N_MATRIX_SIZE 9

//Единичная матрица
static const float E[] = {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f
};

//Метод для формирования единичной матрицы
void getIdentityMatrix(float out_matrix[16]);

//Метод извлечения данных для N-матрицы
void buildNMatrix(float matrix[MVP_MATRIX_SIZE], float out_matrix[9]);
//Метод для транспонирования матрицы
void transpose(float matrix[MVP_MATRIX_SIZE], float out_matrix[16]);

//Метод для преобразования переноса
void move(float matrix[MVP_MATRIX_SIZE], float x, float y, float z, float out_matrix[16]);
//Метод для преобразования масштабирования
void scale(float matrix[MVP_MATRIX_SIZE], float sx, float sy, float sz, float out_matrix[16]);
//Метод для преобразования вращения вокруг X
void rotateAboutX(float matrix[MVP_MATRIX_SIZE], float degree, float out_matrix[16]);
//Метод для преобразования вращения вокруг Y
void rotateAboutY(float matrix[MVP_MATRIX_SIZE], float degree, float out_matrix[16]);
//Метод для преобразования вращения вокруг Z
void rotateAboutZ(float matrix[MVP_MATRIX_SIZE], float degree, float out_matrix[16]);
//Метод для преобразования вращения (|(x, y, z)| = 1)
void rotate(float matrix[MVP_MATRIX_SIZE], float x, float y, float z, float degree, float out_matrix[16]);

//Метод для формирования матрицы параллельной проекции
void getParallelProjectionMatrix(float l, float r, float b, float t, float n, float f, float out_matrix[16]);
//Метод для формирования матрицы перспективной проекции
void getPerspectiveProjectionMatrix(float l, float r, float b, float t, float n, float f, float out_matrix[16]);
//Метод для формирования матрицы перспективной проекции
void getPerspectiveProjectionMatrixByAngle(float n, float f, float w, float h, float fovAngle, float out_matrix[16]);

//Сравнение матриц на равенство
bool equals(float matrixMVP_MATRIX_SIZE[MVP_MATRIX_SIZE], float otherMatrix[MVP_MATRIX_SIZE]);

//Оператор сложения матриц
void add(float matrix[MVP_MATRIX_SIZE], float otherMatrix[MVP_MATRIX_SIZE], float out_matrix[16]);
//Оператор вычитания матриц
void substract(float minuend[MVP_MATRIX_SIZE], float subtrahend[MVP_MATRIX_SIZE], float out_matrix[16]);

//Оператор умножения матриц
void multiplyMatrices(float matrix[MVP_MATRIX_SIZE], float otherMatrix[MVP_MATRIX_SIZE], float out_matrix[16]);
//Оператор сложения матриц
void multiplyByNumber(float matrix[MVP_MATRIX_SIZE], float num, float out_matrix[16]);
//Оператор сложения матриц
void divideByNumber(float matrix[MVP_MATRIX_SIZE], float num, float out_matrix[16]);
#endif //MVP_MATRIX
