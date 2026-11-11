#include "Explosion.h"

#include <iostream>

Explosion::Explosion()
{
    tipo = OBJ_EXPLOSION;
    active = true;
}

void Explosion::init(
    const char* frsFile,
    vec4float startPosition,
    float size
)
{
    if (!loadFromFile(frsFile))
    {
        std::cout << "ERROR: no se pudo cargar explosion desde " << frsFile << "\n";
        active = false;
        return;
    }

    position = startPosition;
    position.w = 1.0f;

    initialSize = size;
    finalSize = size * 2.5f;

    scale = { initialSize, initialSize, initialSize, 0.0f };

    lifeTime = maxLifeTime;

    tipo = OBJ_EXPLOSION;
    active = true;

    // La explosión no necesita participar en colisiones.
    // Podemos dejar su collider creado, pero no se usará porque su tipo
    // no se consulta en getCollisions.
    updateModelMatrix();

    std::cout << "Explosion creada\n";
}

void Explosion::move(double timeStep)
{
    float dt = static_cast<float>(timeStep);

    lifeTime -= dt;

    if (lifeTime <= 0.0f)
    {
        active = false;
        return;
    }

    // t va de 0 a 1 durante la vida de la explosión.
    float t = 1.0f - (lifeTime / maxLifeTime);

    float currentSize = initialSize + (finalSize - initialSize) * t;

    scale = {
        currentSize,
        currentSize,
        currentSize,
        0.0f
    };

    // Rotación visual rápida para que no parezca un sprite estático.
    rotation.z += 6.0f * dt;

    // Si tu shader ya usa material.alpha, podemos hacer fade out.
    if (mat != nullptr)
    {
        mat->alpha = 1.0f - t;
    }

    updateModelMatrix();
}