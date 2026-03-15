#define _USE_MATH_DEFINES

#include <iostream>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <vector>
#include <cmath>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include "Ellipsoid.h"
#include "LightController.h"
#include "Scene.h"
#include "math/Matrix4x4.h"
#include "MouseController.h";

const char* vertSrc = R"(#version 330 core
layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aUV;
out vec2 UV;
void main(){ gl_Position=vec4(aPos,0,1); UV=aUV; })";

const char* fragSrc = R"(#version 330 core
in vec2 UV; out vec4 col;
uniform sampler2D tex;
void main(){ col=texture(tex,UV); })";

static unsigned int compileShader(unsigned int type, const char* src) {
    unsigned int s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr); glCompileShader(s);
    return s;
}

GLFWwindow* initWindow(int H, int W);

int main() {
    const int W = 2000, H = 1600;
    GLFWwindow* win;
    
    if ((win = initWindow(H, W)) == nullptr) return -1;

    // Fullscreen quad
    float verts[] = {
        -1, 1, 0,1,  -1,-1, 0,0,  1,-1, 1,0,
        -1, 1, 0,1,   1,-1, 1,0,  1, 1, 1,1
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO); glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);          
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertSrc);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLuint tex;
    glGenTextures(1, &tex); glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    std::vector<unsigned char> pixels(3 * W * H);
    std::vector<float> zBuffer(W * H, 1e10f);

    Ellipsoid el;
    Scene scene;

    float shininess = 1.0f;
    float radii[3] = { 0.5, 0.5, 0.5 };
    float scale[3] = { 1, 1, 1 };
    int minRenderStep = 1;
    int renderStep = 1;
    bool changed = true;
    LightController light(W, H);
    Matrix4x4 M, Minv, Dp;

    while (!glfwWindowShouldClose(win)) {
        bool isInteracting = MouseController::g_leftDown || 
            MouseController::g_rightDown || 
            ImGui::IsAnyItemActive() ||
            MouseController::g_scrolled;
        
        MouseController::g_scrolled = false;

        if (isInteracting) {
            changed = true;
            renderStep = std::max(8, minRenderStep);
        }
        else if (renderStep > minRenderStep) {
            renderStep /= 2;
        }

        ImGui_ImplOpenGL3_NewFrame(); 
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Sterowanie");
        
        changed |= ImGui::SliderFloat3("Promienie elipsoidy", radii, 0.3, 2.0f);
        changed |= ImGui::SliderFloat("Parametr m", &shininess, 0.3f, 4.0f);
        changed |= ImGui::SliderFloat("Obrot (OX)", &MouseController::g_rotX, 0, 2 * M_PI);
        changed |= ImGui::SliderFloat("Obrot (OY)", &MouseController::g_rotY, 0, 2 * M_PI);
        changed |= ImGui::SliderFloat("Obrot (OZ)", &MouseController::g_rotZ, 0, 2 * M_PI);
        changed |= ImGui::SliderInt("Dokladnosc", &minRenderStep, 1, 20);
        changed |= ImGui::SliderFloat("Zoom", &MouseController::g_zoom, 0.1f, 5.0f);

        ImGui::Text("Obrot: Lewy przycisk myszy (dla osi Z + L Ctrl)");
        ImGui::Text("Przesuniecie: Prawy przycisk myszy");
        ImGui::End();

        if (changed) {
            std::fill(pixels.begin(), pixels.end(), 0);
            std::fill(zBuffer.begin(), zBuffer.end(), 1e10f);

            scene.rotationX = MouseController::g_rotX;
            scene.rotationY = MouseController::g_rotY;
            scene.rotationZ = MouseController::g_rotZ;
            scene.translationX = MouseController::g_tx;
            scene.translationY = MouseController::g_ty;
            scene.translationZ = MouseController::g_tz;
            scene.scaleX = MouseController::g_zoom;
            scene.scaleY = MouseController::g_zoom;
            scene.scaleZ = MouseController::g_zoom;

            el.radii = { radii[0], radii[1], radii[2] };

            // Macierz M i M^-1
            M = scene.modelMatrix();
            Minv = scene.modelMatrixInverse();
            Dp = el.buildDprime(Minv);

            // Kolorowanie
            light.fillPixels(renderStep, el, Dp, pixels, shininess);
            
        }

        glBindTexture(GL_TEXTURE_2D, tex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, W, H, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(prog); glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(win); 
        glfwPollEvents();

        if (renderStep <= minRenderStep) {
            changed = false;
        }
    }

    glDeleteVertexArrays(1, &VAO); 
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

GLFWwindow* initWindow(int H, int W) {
    if (!glfwInit()) return nullptr;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(W, H, "Wizualizacja elipsoidy zapisanej w postaci implicite", nullptr, nullptr);
    if (!win) {
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(win);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return nullptr;

    glfwSetMouseButtonCallback(win, MouseController::onSetMouseButtonCallback);
    glfwSetCursorPosCallback(win, MouseController::onSetCursorPosCallback);
    glfwSetScrollCallback(win, MouseController::onScrollCallback);

    // Fullscreen quad
    float verts[] = {
        -1, 1, 0,1,  -1,-1, 0,0,  1,-1, 1,0,
        -1, 1, 0,1,   1,-1, 1,0,  1, 1, 1,1
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO); glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertSrc);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragSrc);
    unsigned int prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLuint tex;
    glGenTextures(1, &tex); glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);


    // ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return win;
}