#include "InputManager.h"
#include <GLFW/glfw3.h>

// Inicialización de variables estáticas
std::map<int, bool> InputManager::keyState;
std::map<int, bool> InputManager::mouseState;

double InputManager::posMX = 0.0;
double InputManager::posMY = 0.0;

// Callback teclado
void InputManager::keyboardManager(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_UNKNOWN) return;
    if (action == GLFW_PRESS || action == GLFW_REPEAT) keyState[key] = true;
    if (action == GLFW_RELEASE)                        keyState[key] = false;
}

// Callback posición del ratón
void InputManager::mousePosManager(GLFWwindow* window,
    double xpos,
    double ypos)
{
    // Guardamos la posición actual del cursor
    posMX = xpos;
    posMY = ypos;
}


// Callback botones del ratón
void InputManager::mouseButtonManager(GLFWwindow* window,
    int button,
    int action,
    int mods)
{
    if (action == GLFW_PRESS)
    {
        mouseState[button] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        mouseState[button] = false;
    }
}

// Inicializacion, limpiar estados y registrar callbacks
void InputManager::init(GLFWwindow* window)
{
    keyState.clear();
    mouseState.clear();
    posMX = posMY = 0.0;

    glfwSetKeyCallback(window, InputManager::keyboardManager);
    glfwSetCursorPosCallback(window, InputManager::mousePosManager);
    glfwSetMouseButtonCallback(window, InputManager::mouseButtonManager);
}

void InputManager::update()
{
    glfwPollEvents();
}