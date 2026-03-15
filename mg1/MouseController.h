#pragma once
#include <glfw/glfw3.h>
#include <imgui/imgui.h>

class MouseController {
private:
    inline static const float multiplicator = 0.005f;

public:
    inline static bool   g_leftDown = false;
    inline static bool   g_rightDown = false;
    inline static float  g_lastX = 0, g_lastY = 0;
    inline static float  g_rotX = 0, g_rotY = 0, g_rotZ = 0;
    inline static float  g_tx = 0, g_ty = 0, g_tz = 0;
    inline static float  g_zoom = 1;
    inline static bool   g_scrolled = false;

    static void onSetCursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
        // Blokuj ruch sceny gdy ImGui obs³uguje myszkê (suwaki, okna UI)
        if (ImGui::GetIO().WantCaptureMouse) return;

        float dx = xpos - g_lastX;
        float dy = ypos - g_lastY;

        bool ctrlPressed = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);

        if (g_leftDown) {
            if (ctrlPressed) {
                g_rotZ += dx * multiplicator;
                if (g_rotZ < 0) g_rotZ += 2 * M_PI;
                if (g_rotZ > 2 * M_PI) g_rotZ -= 2 * M_PI;
            }
            else {
                g_rotY += dx * multiplicator;
                g_rotX += dy * multiplicator;

                if (g_rotY < 0) g_rotY += 2 * M_PI;
                if (g_rotX < 0) g_rotX += 2 * M_PI;

                if (g_rotY > 2 * M_PI) g_rotY -= 2 * M_PI;
                if (g_rotX > 2 * M_PI) g_rotX -= 2 * M_PI;

            }
        }
        if (g_rightDown) {
            g_tx += dx * multiplicator;
            g_ty += dy * multiplicator;
        }

        g_lastX = xpos;
        g_lastY = ypos;
    }

    static void onSetMouseButtonCallback(GLFWwindow*, int btn, int action, int) {
        // Wyznacza wciœniêty przycisk myszy (o ile nie jest nad oknem sterowania)

        if (ImGui::GetIO().WantCaptureMouse) {
            g_leftDown = false;
            g_rightDown = false;
            return;
        }

        if (btn == GLFW_MOUSE_BUTTON_LEFT) {
            g_leftDown = (action == GLFW_PRESS);
        }
        if (btn == GLFW_MOUSE_BUTTON_RIGHT) {
            g_rightDown = (action == GLFW_PRESS);
        }
    }

    static void onScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        g_zoom += (float)yoffset * 0.1f;
        g_zoom = std::min(std::max(0.1f, g_zoom), 5.0f);
        g_scrolled = true;
    }
};
