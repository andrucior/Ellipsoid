#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include "math/Vector3.h"
#include "math/Matrix4x4.h"
#include "Ellipsoid.h"

class LightController {
private:
    Vector3 cameraPos = { 0,0, 30 };

    int width, height;

    // Gradient f(u) = u^T D u => grad = 2 D u
    Vector3 computeNormal(const Matrix4x4& Dp, const Vector4& p) const {
        Vector4 vec = Dp * p;
        return Vector3(vec.x, vec.y, vec.z).normalize();
    }

public:
    LightController(int width, int height)
        : width(width), height(height) {
    }

    float intensity(const Vector3& v, const Vector3& n, float m) const {
        float dot = std::max(0.0f, v.dot(n));
        return std::pow(dot, m);
    }

    void fillPixel(int x, int y, const Ellipsoid& el, const Matrix4x4& Dp,
        std::vector<unsigned char>& pixels, float m) const
    {
        // Uklad wspolrzednych
        float screenX = (x - width * 0.5f) / (height * 0.5f);
        float screenY = -((y - height * 0.5f) / (width * 0.5f));

        // Promien od kamery wglab 
        Ray ray;
        ray.origin = { screenX, screenY, cameraPos.z };
        ray.direction = { 0.0f, 0.0f, -1.0f };

        float t = el.intersect(ray, Dp);
        int pixelIdx = y * width + x;

        if (t > 0.0f) {
            Vector3 hit = ray.origin + ray.direction * t;

            Vector3 v = (cameraPos - hit).normalize();
            Vector3 n = computeNormal(Dp, hit);
            float I = intensity(v, n, m);
            unsigned char c = (unsigned char)(255.0f * std::min(1.0f, I));

            pixels[pixelIdx * 3] = c;
            pixels[pixelIdx * 3 + 1] = c;
            pixels[pixelIdx * 3 + 2] = 0;

         
        }
        else {
            pixels[pixelIdx * 3] = 0;
            pixels[pixelIdx * 3 + 1] = 0;
            pixels[pixelIdx * 3 + 2] = 0;
        }
    }

    void fillPixels(
        int renderStep, 
        Ellipsoid el, 
        Matrix4x4 Dp, 
        std::vector<unsigned char>& pixels, 
        float m
    ) const {
        for (int y = 0; y < height; y += renderStep) {
            for (int x = 0; x < width; x += renderStep) {

                fillPixel(x, y, el, Dp, pixels, m);

                int baseIdx = (y * width + x) * 3;
                unsigned char r = pixels[baseIdx];
                unsigned char g = pixels[baseIdx + 1];
                unsigned char b = pixels[baseIdx + 2];

                // Kopiujemy  kolor do reszty pikseli w obecnym bloku 
                if (renderStep > 1) {
                    for (int dy = 0; dy < renderStep && (y + dy) < height; ++dy) {
                        for (int dx = 0; dx < renderStep && (x + dx) < width; ++dx) {
                            if (dx == 0 && dy == 0) continue;

                            int idx = ((y + dy) * width + (x + dx)) * 3;
                            pixels[idx] = r;
                            pixels[idx + 1] = g;
                            pixels[idx + 2] = b;
                        }
                    }
                }
            }
        }
    }
};