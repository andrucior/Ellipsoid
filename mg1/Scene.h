#pragma once
#include <iostream>
#include "math/Matrix4x4.h"

// Przechowuje aktualny stan transformacji sceny 
class Scene {
public:
    float translationX = 0, translationY = 0, translationZ = 0;
    float rotationX = 0, rotationY = 0, rotationZ = 0;
    float scaleX = 1, scaleY = 1, scaleZ = 1;

    // Buduje M = T * Rx * Ry * Rz * S
    Matrix4x4 modelMatrix() const {
        return Matrix4x4::translate(translationX, translationY, translationZ)
            * Matrix4x4::rotateX(rotationX)
            * Matrix4x4::rotateY(rotationY)
            * Matrix4x4::rotateZ(rotationZ)
            * Matrix4x4::scale(scaleX, scaleY, scaleZ);
    }

    Matrix4x4 rotationMatrix() const {
        return Matrix4x4::rotateX(rotationX)
            * Matrix4x4::rotateY(rotationY)
            * Matrix4x4::rotateZ(rotationZ);
    }

    Matrix4x4 modelMatrixInverse() const {
        return Matrix4x4::scale(1 / scaleX, 1 / scaleY, 1 / scaleZ)
            * Matrix4x4::rotateX(rotationX).Transposed()
            * Matrix4x4::rotateY(rotationY).Transposed()
            * Matrix4x4::rotateZ(rotationZ).Transposed()
            * Matrix4x4::translate(-translationX, -translationY, -translationZ);
    }
};