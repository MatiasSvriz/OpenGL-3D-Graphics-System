#include "Planet.h"

#include <iostream>
#include <cmath>

Planet::Planet()
{
    tipo = OBJ_PLANET;
    active = true;
}

void Planet::init(
    const char* frsFile,
    vec4float startPosition,
    float size
)
{
    // Cargamos el planeta desde FRS.
    // El FRS debe contener geometría, textura y shaders.
    if (!loadFromFile(frsFile))
    {
        std::cout << "ERROR: no se pudo cargar planeta desde " << frsFile << "\n";
        active = false;
        return;
    }

    position = startPosition;
    position.w = 1.0f;

    scale = { size, size, size, 0.0f };

    tipo = OBJ_PLANET;
    active = true;
    health = 3;

    updateModelMatrix();

    std::cout << "Planeta creado desde " << frsFile << "\n";
}

void Planet::hit()
{
    health--;

    std::cout << "Planeta impactado. Vida restante: " << health << "\n";

    if (health <= 0)
    {
        std::cout << "Planeta destruido\n";
        active = false;
    }
}

void Planet::move(double timeStep)
{
    float dt = static_cast<float>(timeStep);

    if (orbit)
    {
        orbitAngle += orbitSpeed * dt;

        position = make_rotateX(orbitAngle);
        //position.x = orbitCenter.x + cosf(orbitAngle) * orbitRadius;
        //position.y = orbitCenter.y + sinf(orbitAngle) * orbitRadius;
        //position.z = orbitCenter.z;
        //position.w = 1.0f;
    }

    // Rotación visual del planeta.
    rotation.z += 0.6f * dt;

    updateModelMatrix();
}