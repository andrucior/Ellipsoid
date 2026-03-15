#pragma once
#include <array>
#include <cmath>
#include <iomanip>
#include "Vector4.h"

class Matrix4x4 {
public:
    std::array<std::array<float, 4>, 4> m;

    Matrix4x4(std::array<std::array<float, 4> , 4> m) : m(m) { }

    Matrix4x4() {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = 0.0f;
    }

    static Matrix4x4 Identity() {
        std::array<std::array<float, 4>, 4> m;
        
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
        
        return Matrix4x4(m);
    }

    // Dostêp do elementów
    float* operator[](int row) { return m[row].data(); }
    const float* operator[](int row) const { return m[row].data(); }

    // Mno¿enie macierzy
    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result = Identity();
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result[i][j] = 0;
                for (int k = 0; k < 4; k++)
                    result[i][j] += m[i][k] * other[k][j];
            }
        }
        return result;
    }

    // Mno¿enie macierzy przez wektor 
    Vector4 operator*(const Vector4& v) const {
        float x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
        float y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
        float z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
        float w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;

        return Vector4(x, y, z, w);
    }

    Vector4 multiplyRowVector(const Vector4& v) const {
        return Vector4(
            v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0],
            v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1],
            v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2],
            v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3]
        );
    }

    // Macierz translacji
    static Matrix4x4 translate(float tx, float ty, float tz) {
        Matrix4x4 result = Identity();
        result[0][3] = tx;
        result[1][3] = ty;
        result[2][3] = tz;
        return result;
    }

    // Macierz skalowania
    static Matrix4x4 scale(float sx, float sy, float sz) {
        Matrix4x4 result = Identity();
        result[0][0] = sx;
        result[1][1] = sy;
        result[2][2] = sz;
        return result;
    }

    // Macierz obrotu wokó³ osi X
    static Matrix4x4 rotateX(float angle) {
        Matrix4x4 result = Identity();
        float c = std::cos(angle);
        float s = std::sin(angle);
        result[1][1] = c;  result[1][2] = -s;
        result[2][1] = s;  result[2][2] = c;
        return result;
    }

    // Macierz obrotu wokó³ osi Y
    static Matrix4x4 rotateY(float angle) {
        Matrix4x4 result = Identity();
        float c = std::cos(angle);
        float s = std::sin(angle);
        result[0][0] = c;  result[0][2] = s;
        result[2][0] = -s; result[2][2] = c;
        return result;
    }

    // Macierz obrotu wokó³ osi Z
    static Matrix4x4 rotateZ(float angle) {
        Matrix4x4 result = Identity();
        float c = std::cos(angle);
        float s = std::sin(angle);
        result[0][0] = c;  result[0][1] = -s;
        result[1][0] = s;  result[1][1] = c;
        return result;
    }

    // Odwrócenie macierzy
    static Matrix4x4 inverse(const Matrix4x4& mat) {
        float aug[4][8];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) aug[i][j] = mat[i][j];
            for (int j = 0; j < 4; j++) aug[i][j + 4] = (i == j) ? 1.0f : 0.0f;
        }
        for (int col = 0; col < 4; col++) {
            // Pivot
            int pivot = col;
            for (int row = col + 1; row < 4; row++)
                if (std::abs(aug[row][col]) > std::abs(aug[pivot][col]))
                    pivot = row;
            std::swap(aug[col], aug[pivot]);

            float div = aug[col][col];
            for (int j = 0; j < 8; j++) aug[col][j] /= div;

            for (int row = 0; row < 4; row++) {
                if (row == col) continue;
                float factor = aug[row][col];
                for (int j = 0; j < 8; j++)
                    aug[row][j] -= factor * aug[col][j];
            }
        }
        Matrix4x4 result;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                result[i][j] = aug[i][j + 4];
        return result;
    }

    Matrix4x4 Transposed() const {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[j][i] = m[i][j];
            }
        }
        return result;
    }
    
    void print() const {
        for (int i = 0; i < 4; i++) {
            std::cout << "[ ";
            for (int j = 0; j < 4; j++) {
                std::cout << std::setw(8) << m[i][j];
                if (j < 3) std::cout << ", ";
            }
            std::cout << " ]\n";
        }
    }
};