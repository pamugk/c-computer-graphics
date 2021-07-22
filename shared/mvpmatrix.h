#include <stdbool.h>

// Размерность MVP-матрицы
const unsigned char MVP_MATRIX_DIMENSION_SIZE = 4;
const unsigned char MVP_MATRIX_SIZE = MVP_MATRIX_DIMENSION_SIZE * MVP_MATRIX_DIMENSION_SIZE;

// Размерность N-матрицы
static const unsigned char N_MATRIX_DIMENSION_SIZE = 3;
static const unsigned char N_MATRIX_SIZE = N_MATRIX_DIMENSION_SIZE * N_MATRIX_DIMENSION_SIZE;

//Единичная матрица
static const float E[] = {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f
};

//Метод извлечения данных для N-матрицы
void buildNMatrix(float matrix[16], float **out_matrix);
//Метод для транспонирования матрицы
void transpose(float matrix[16], float **out_matrix);

//Метод для преобразования переноса
void move(float matrix[16], float x, float y, float z, float **out_matrix);
//Метод для преобразования масштабирования
void scale(float matrix[16], float sx, float sy, float sz, float **out_matrix);
//Метод для преобразования вращения вокруг X
void rotateAboutX(float matrix[16], float degree, float **out_matrix);
//Метод для преобразования вращения вокруг Y
void rotateAboutY(float matrix[16], float degree, float **out_matrix);
//Метод для преобразования вращения вокруг Z
void rotateAboutZ(float matrix[16], float degree, float **out_matrix);
//Метод для преобразования вращения (|(x, y, z)| = 1)
void rotate(float matrix[16], float x, float y, float z, float degree, float **out_matrix);

//Метод для формирования матрицы параллельной проекции
void getParallelProjectionMatrix(float l, float r, float b, float t, float n, float f, float **out_matrix);
//Метод для формирования матрицы перспективной проекции
void getPerspectiveProjectionMatrix(float l, float r, float b, float t, float n, float f, float **out_matrix);
//Метод для формирования матрицы перспективной проекции
void getPerspectiveProjectionMatrixByAngle(float n, float f, float w, float h, float fovAngle, float **out_matrix);

//Сравнение матриц на равенство
bool equals(float matrix[16], float otherMatrix[16]);

//Оператор сложения матриц
void add(float matrix[16], float otherMatrix[16], float **out_matrix);
//Оператор вычитания матриц
void substract(float minuend[16], float subtrahend[16], float **out_matrix);

//Оператор умножения матриц
void multiplyMatrices(float matrix[16], float otherMatrix[16], float **out_matrix);
//Оператор сложения матриц
void multiplyByNumber(float matrix[16], float num, float **out_matrix);
//Оператор сложения матриц
void divideByNumber(float matrix[16], float num, float **out_matrix);
