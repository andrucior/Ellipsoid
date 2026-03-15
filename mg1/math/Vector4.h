#pragma once
#include <cmath>
#include <iostream>
#include <stdexcept>
#include "Vector3.h"
#include "Matrix4x4.h"

class Vector4 {
public:
    float x, y, z, w;
    Vector4(float x = 0, float y = 0, float z = 0, float w = 0) : x(x), y(y), z(z), w(w) {}

    // Konwersja z Vector3 (punkt: w=1, kierunek: w=0)
    Vector4(const Vector3& v, float w = 1.0f) : x(v.x), y(v.y), z(v.z), w(w) {}

    // Powrót do Vector3 (ignoruje w)
    Vector3 xyz() const { return Vector3(x, y, z); }

    Vector4 operator+(const Vector4& v)   const { return { x + v.x, y + v.y, z + v.z, w + v.w }; }
    Vector4 operator-(const Vector4& v)   const { return { x - v.x, y - v.y, z - v.z, w - v.w }; }
    Vector4 operator*(float s)            const { return { x * s,   y * s,   z * s,   w * s }; }
    Vector4 operator-()                   const { return { -x, -y, -z, -w }; }
   
    float operator[](int i) const {
        switch (i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default: throw std::out_of_range("Index out of range");
        }
    }

    float& operator[](int i) {
        switch (i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default: throw std::out_of_range("Index out of range");
        }
    }

    float dot(const Vector4& v) const {
        return x * v.x + y * v.y + z * v.z + w * v.w;
    }

    float length() const {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    Vector4 normalize() const {
        float len = length();
        if (len == 0) return {};
        return (*this) * (1.0f / len);
    }

    void print() const {
        std::cout << "(" << x << ", " << y << ", " << z << ", " << w << ")\n";
    }
};