#pragma once
#include <map>
#include <GLFW/glfw3.h>

class InputManager
{
public:
    // Teclado
    static std::map<int, bool> keyState;
    static void keyboardManager(GLFWwindow* window, int key, int scancode, int action, int mods);

    // Raton
    static std::map<int, bool> mouseState;  // estado botones
    static double posMX;                    // posicion X del cursor
    static double posMY;                    // posicion Y del cursor

    static void mousePosManager(GLFWwindow* window, double x, double y);
    static void mouseButtonManager(GLFWwindow* window, int button, int action, int mods);

    static void init(GLFWwindow* window);
    static void update();
};