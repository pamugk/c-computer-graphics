#include "vectormath.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

// Вычисление нормали (без нормализации)
void calculateNormal(float pointA[3], float pointB[3], float pointC[3], float multiplier, float normal[3]) {
    float AB[3] = { pointB[0] - pointA[0], pointB[1] - pointA[1], pointB[2] - pointA[2] };
    float lenAB = sqrt(AB[0] * AB[0] + AB[1] * AB[1] + AB[2] * AB[2]);
    
    float AC[3] = { pointC[0] - pointA[0], pointC[1] - pointA[1], pointC[2] - pointA[2] };
    float lenAC = sqrt(AC[0] * AC[0] + AC[1] * AC[1] + AC[2] * AC[2]);
    
    float cosV = (AB[0] * AC[0] + AB[1] * AC[1] + AB[2] * AC[2]) / lenAB / lenAC;
    
    normal[0] =  multiplier * cosV * (AB[1] * AC[2] - AC[1] * AB[2]);
    normal[1] = multiplier * cosV * (-AB[0] * AC[2] + AC[2] * AB[0]);
    normal[2] =  multiplier * cosV * (AB[0] * AC[1] - AC[0] * AB[1]);
}

// Нормализация
void normalize(float vector[3]) {
    float len = sqrt(vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2]);
    vector[0] /= len; vector[1] /= len; vector[2] /= len;
}

//Метод для формирования единичной матрицы
void getIdentityMatrix(float out_matrix[16]) {
    memcpy(out_matrix, E, sizeof(float) * MVP_MATRIX_SIZE);
}

//Метод извлечения данных для N-матрицы
void buildNMatrix(float matrix[MVP_MATRIX_SIZE], float out_matrix[9]) {
    float m[N_MATRIX_SIZE];
    for (int i = 0; i < N_MATRIX_DIMENSION_SIZE; i += 1) {
        for (int j = 0; j < N_MATRIX_DIMENSION_SIZE; j += 1) {
            m[i * N_MATRIX_DIMENSION_SIZE + j] = matrix[i * MVP_MATRIX_DIMENSION_SIZE + j];
        }
    }
    
    float det = m[0] * m[4] * m[8] + m[3] * m[7] * m[2] + m[1] * m[5] * m[6] - 
                m[6] * m[4] * m[2] - m[1] * m[3] * m[8] - m[5] * m[7] * m[0];
                
    out_matrix[0] = (m[4] * m[8] - m[5] * m[7]) / det;
    out_matrix[1] = (m[7] * m[2] - m[1] * m[8]) / det;
    out_matrix[2] = (m[1] * m[5] - m[4] * m[2]) / det;
    
    out_matrix[3] = (m[6] * m[5] - m[3] * m[8]) / det;
    out_matrix[4] = (m[0] * m[8] - m[2] * m[6]) / det;
    out_matrix[5] = (m[3] * m[2] - m[0] * m[5]) / det;
    
    out_matrix[6] = (m[3] * m[7] - m[6] * m[4]) / det;
    out_matrix[7] = (m[1] * m[6] - m[0] * m[7]) / det;
    out_matrix[8] = (m[0] * m[4] - m[3] * m[1]) / det;
}

//Метод для транспонирования матрицы
void transpose(float matrix[MVP_MATRIX_SIZE], float out_matrix[16]) {
    for (int i = 0; i < MVP_MATRIX_DIMENSION_SIZE; i += 1) {
        for (int j = 0; j < MVP_MATRIX_DIMENSION_SIZE; j += 1) {
            out_matrix[i * MVP_MATRIX_DIMENSION_SIZE + j] = matrix[j * MVP_MATRIX_DIMENSION_SIZE + i];
        }
    }
}

//Метод для преобразования переноса
void move(float matrix[MVP_MATRIX_SIZE], float x, float y, float z, float out_matrix[16]) {
    float moveMatrix[] = {
        1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
        x,  y,  z,  1.f
    };
    multiplyMatrices(moveMatrix, matrix, out_matrix);
}

//Метод для преобразования масштабирования
void scale(float matrix[MVP_MATRIX_SIZE], float sx, float sy, float sz, float out_matrix[16]) {
    float moveMatrix[] = { 
        sx, 0.f, 0.f, 0.f,
		0.f, sy, 0.f, 0.f,
		0.f, 0.f, sz, 0.f,
		0.f, 0.f, 0.f, 1.f
    };
    multiplyMatrices(moveMatrix, matrix, out_matrix);
}

//Метод для преобразования вращения вокруг X
void rotateAboutX(float matrix[MVP_MATRIX_SIZE], float degree, float out_matrix[16]) {
    rotate(matrix, 1.f, 0.f, 0.f, degree, out_matrix);
}

//Метод для преобразования вращения вокруг Y
void rotateAboutY(float matrix[MVP_MATRIX_SIZE], float degree, float out_matrix[16]) {
    rotate(matrix, 0.f, 1.f, 0.f, degree, out_matrix);
}

//Метод для преобразования вращения вокруг Z
void rotateAboutZ(float matrix[MVP_MATRIX_SIZE], float degree, float out_matrix[16]) {
    rotate(matrix, 0.f, 0.f, 1.f, degree, out_matrix);
}

//Метод для преобразования вращения (|(x, y, z)| = 1)
void rotate(float matrix[MVP_MATRIX_SIZE], float x, float y, float z, float degree, float out_matrix[16]) {
    float c = cos(degree);
    float s = sin(degree);
    float rotateMatrix[] = {
        x * x * (1.f - c) + c, y * x * (1.f - c) + z * s, x * z * (1.f - c) - y * s, 0.f,
        x * y * (1.f - c) - z * s, y * y * (1.f - c) + c, y * z * (1.f - c) + x * s, 0.f,
        x * z * (1.f - c) + y * s, y * z * (1.f - c) - x * s, z * z * (1.f - c) + c, 0.f,
        0.f, 0.f, 0.f, 1.f
    };
    multiplyMatrices(rotateMatrix, matrix, out_matrix);
}

//Метод для формирования матрицы параллельной проекции
void getParallelProjectionMatrix(float l, float r, float b, float t, float n, float f, float out_matrix[16]) {
    out_matrix[0] = 2.f / (r - l);
    out_matrix[1] = 0.f;
    out_matrix[2] = 0.f;
    out_matrix[3] = 0.f;
    
    out_matrix[4] = 0.f;
    out_matrix[5] = 2.f / (t - b);
    out_matrix[6] = 0.f;
    out_matrix[7] = 0.f;
    
    out_matrix[8] = 0.f;
    out_matrix[9] = 0.f;
    out_matrix[10] = -2.f / (f - n);
    out_matrix[11] = 0.f;
    
    out_matrix[12] = -(r+l)/(r-l);
    out_matrix[13] = -(t+b)/(t-b);
    out_matrix[14] = -(f+n)/(f-n);
    out_matrix[15] = 1.f;
}

//Метод для формирования матрицы перспективной проекции
void getPerspectiveProjectionMatrix(float l, float r, float b, float t, float n, float f, float out_matrix[16]) {
    out_matrix[0] = 2.f * n / (r - l);
    out_matrix[1] = 0.f;
    out_matrix[2] = 0.f;
    out_matrix[3] = 0.f;
    
    out_matrix[4] = 0.f;
    out_matrix[5] = 2.f * n / (t - b);
    out_matrix[6] = 0.f;
    out_matrix[7] = 0.f;
    
    out_matrix[8] = (r + l) / (r - l);
    out_matrix[9] = (t + b) / (t - b);
    out_matrix[10] = -(f + n) / (f - n);
    out_matrix[11] = -1.f;
    
    out_matrix[12] = 0.f;
    out_matrix[13] = 0.f;
    out_matrix[14] = -2.f *f * n / (f - n);
    out_matrix[15] = 1.f;
}

//Метод для формирования матрицы перспективной проекции
void getPerspectiveProjectionMatrixByAngle(float n, float f, float w, float h, float fovAngle, float out_matrix[16]) {
    float tg = tanf(M_PI_2 / 180.f * fovAngle);
    getPerspectiveProjectionMatrix(-n * tg, n * tg, -n * w / h * tg, n * w / h * tg, n, f, out_matrix);
}

//Сравнение матриц на равенство
bool equals(float matrix[MVP_MATRIX_SIZE], float otherMatrix[MVP_MATRIX_SIZE]) {
    for (int i = 0; i < MVP_MATRIX_SIZE; i += 1) {
        if (matrix[i] != otherMatrix[i]) {
            return false;
        }
    }
    return true;
}

//Оператор сложения матриц
void add(float matrix[MVP_MATRIX_SIZE], float otherMatrix[MVP_MATRIX_SIZE], float out_matrix[16]) {
    for (int i = 0; i < MVP_MATRIX_SIZE; i += 1) {
        out_matrix[i] = matrix[i] + otherMatrix[i];
    }
}

//Оператор вычитания матриц
void substract(float minuend[MVP_MATRIX_SIZE], float subtrahend[MVP_MATRIX_SIZE], float out_matrix[16]) {
    for (int i = 0; i < MVP_MATRIX_SIZE; i += 1) {
        out_matrix[i] = minuend[i] - subtrahend[i];
    }
}

//Оператор умножения матриц
void multiplyMatrices(float matrix[MVP_MATRIX_SIZE], float otherMatrix[MVP_MATRIX_SIZE], float out_matrix[16]) {
    for(int i = 0; i < MVP_MATRIX_DIMENSION_SIZE; i += 1) {
        for (int j = 0; j < MVP_MATRIX_DIMENSION_SIZE; j += 1) {
            float sum = 0;
            for (int k = 0; k < MVP_MATRIX_DIMENSION_SIZE; k += 1) {
                sum += matrix[k * MVP_MATRIX_DIMENSION_SIZE + i] * otherMatrix[j * MVP_MATRIX_DIMENSION_SIZE + k];
            }
            out_matrix[j * MVP_MATRIX_DIMENSION_SIZE + i] = sum;
        }
    }
}

//Оператор умножения матрицы на число
void multiplyMatrixByNumber(float matrix[MVP_MATRIX_SIZE], float num, float out_matrix[16]) {
    for (int i = 0; i < MVP_MATRIX_SIZE; i += 1) {
        out_matrix[i] = matrix[i] * num;
    }
}

//Оператор деления матрицы на число
void divideMatrixByNumber(float matrix[MVP_MATRIX_SIZE], float num, float out_matrix[16]) {
    for (int i = 0; i < MVP_MATRIX_SIZE; i += 1) {
        out_matrix[i] = matrix[i] / num;
    }
}

bool vec3ubEqual(unsigned char thisVector[3], unsigned char thatVector[3]) {
    return thisVector[0] == thatVector[0]
        && thisVector[1] == thatVector[1]
        && thisVector[2] == thatVector[2];
}
