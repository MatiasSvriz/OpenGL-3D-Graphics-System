#include "Sun.h"

#include <iostream>

Sun::Sun()
{
    tipo = OBJ_SUN;
    active = true;
}

void Sun::init(
    const char* frsFile,
    vec4float startPosition,
    float size
)
{
    if (!loadFromFile(frsFile))
    {
        std::cout << "ERROR: no se pudo cargar el sol desde " << frsFile << "\n";
        active = false;
        return;
    }

    position = startPosition;
    position.w = 1.0f;

    scale = { size, size, size, 0.0f };

    tipo = OBJ_SUN;
    active = true;

    updateModelMatrix();

    std::cout << "Sol creado desde " << frsFile << "\n";
}

void Sun::move(double timeStep)
{
    float dt = static_cast<float>(timeStep);

    // Rotación visual suave del Sol.
    rotation.z += 0.4f * dt;

    updateModelMatrix();
}