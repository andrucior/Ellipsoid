#pragma once
#include <vector>
#include "math/Vector3.h"
#include "math/Matrix4x4.h"

class AxisRenderer {
private:
    const int glyphs[3][5][5] = {
        { // X
            {1,0,0,0,1},
            {0,1,0,1,0},
            {0,0,1,0,0},
            {0,1,0,1,0},
            {1,0,0,0,1}
        },
        { // Y
            {0,0,1,0,0},
            {0,0,1,0,0},
            {0,0,1,0,0},
            {0,1,0,1,0},
            {1,0,0,0,1}
        },
        { // Z
            {1,1,1,1,1},
            {0,1,0,0,0},
            {0,0,1,0,0},
            {0,0,0,1,0},
            {1,1,1,1,1}
        }
    };

    void drawGlyph(std::vector<unsigned char>& pixels, int W, int H, int x, int y, int type, const unsigned char color[3]) const {
        int glyphSize = 5;
        for (int i = 0; i < glyphSize; i++) {
            for (int j = 0; j < glyphSize; j++) {
                if (glyphs[type][i][j]) {
                    int fx = x + j;
                    int fy = y + i;
                    if (fx >= 0 && fx < W && fy >= 0 && fy < H) {
                        int idx = (fy * W + fx) * 3;
                        pixels[idx] = color[0];
                        pixels[idx + 1] = color[1];
                        pixels[idx + 2] = color[2];
                    }
                }
            }
        }
    }
    void drawLine(std::vector<unsigned char>& pixels, int W, int H,
        int x0, int y0, int x1, int y1, const unsigned char color[3]) const
    {
        int dx = abs(x1 - x0), dy = abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1;
        int sy = y0 < y1 ? 1 : -1;
        int err = dx - dy;
        while (true) {
            if (x0 >= 0 && x0 < W && y0 >= 0 && y0 < H) {
                int idx = (y0 * W + x0) * 3;
                pixels[idx] = color[0];
                pixels[idx + 1] = color[1];
                pixels[idx + 2] = color[2];
            }
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; x0 += sx; }
            if (e2 < dx) { err += dx; y0 += sy; }
        }
    }
public:
    int thickness = 1;

    void draw(std::vector<unsigned char>& pixels, int W, int H) const {
        const int ox = 60;
        const int oy = H - 60;
        const int len = 40;

        const int ends[3][2] = {
            { ox + len,  oy       },   // X: prawo
            { ox,        oy - len },   // Y: góra
            { ox + len / 2, oy + len / 2 } // Z: prawo-dó³ (skos)
        };

        const unsigned char colors[3][3] = {
            {255, 80,  80 },  // X: czerwony
            { 80, 255,  80},  // Y: zielony
            {100, 180, 255}   // Z: niebieski
        };

        for (int axis = 0; axis < 3; axis++) {
            drawLine(pixels, W, H, ox, oy, ends[axis][0], ends[axis][1], colors[axis]);
            int lx = ends[axis][0] + (ends[axis][0] - ox > 0 ? 3 : -8);
            int ly = ends[axis][1] + (ends[axis][1] - oy > 0 ? 3 : -8);
            drawGlyph(pixels, W, H, lx, ly, axis, colors[axis]);
        }
    }
};