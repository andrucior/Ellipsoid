#pragma once
#include "math/Matrix4x4.h"
#include "math/Vector3.h"

struct Ray {
    Vector3 origin, direction;
};

class Ellipsoid {
public:
    Vector3 radii = { 0.6f, 0.6f, 0.7f };

    // D = diag(1/a^2, 1/b^2, 1/c^2, -1)
    Matrix4x4 buildD() const {
        Matrix4x4 D;
        D[0][0] = 1.0f / (radii.x * radii.x);
        D[1][1] = 1.0f / (radii.y * radii.y);
        D[2][2] = 1.0f / (radii.z * radii.z);
        D[3][3] = -1.0f;
        return D;
    }

    // D'_M = (M^-1)^T * D * M^-1
    Matrix4x4 buildDprime(const Matrix4x4& Minv) const {
        Matrix4x4 D = buildD();
        Matrix4x4 MinvT = Minv.Transposed();
        return MinvT * D * Minv;
    }

    // Przeciêcie promienia z elipsoid¹ 
    float intersect(const Ray& ray, const Matrix4x4& Dp) const {
        // Równanie kwadratowe
        // At^2 + Bt + C = 0
        float ox = ray.origin.x, oy = ray.origin.y, oz = ray.origin.z;
        float dx = ray.direction.x, dy = ray.direction.y, dz = ray.direction.z;

        // A = d^T D'_M d 
        Vector4 Dpd = Dp * Vector4(dx, dy, dz, 0.0f);
        float A = dx * Dpd[0] + dy * Dpd[1] + dz * Dpd[2] + 0.0f * Dpd[3];

        if (std::abs(A) < 1e-8f) return -1.0f;

        // B = 2 o^T D'_M d
        Vector4 Dpo = Dp * Vector4(ox, oy, oz, 1.0f);
        float B = ox * Dpd[0] + oy * Dpd[1] + oz * Dpd[2] + 1.0f * Dpd[3];
        B *= 2.0f;

        // C = o^T D'_M o
        float C = ox * Dpo[0] + oy * Dpo[1] + oz * Dpo[2] + 1.0f * Dpo[3];

        float disc = B * B - 4.0f * A * C;
        if (disc < 0) return -1.0f;

        float sqrtD = std::sqrt(disc);
        float t1 = (-B - sqrtD) / (2.0f * A);
        float t2 = (-B + sqrtD) / (2.0f * A);


        if (t1 > 1e-4f) return t1;
        if (t2 > 1e-4f) return t2;
        return -1.0f;
    }
};