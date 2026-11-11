#include "Bullet.h"

#include <iostream>

Bullet::Bullet()
{
    tipo = OBJ_BULLET;
    active = true;
}

void Bullet::init(
    const char* frsFile,
    vec4float startPosition,
    vec4float direction,
    float speed
)
{
    if (!loadFromFile(frsFile))
    {
        std::cout << "ERROR: no se pudo cargar la bala desde " << frsFile << "\n";
        active = false;
        return;
    }

    position = startPosition;
    position.w = 1.0f;

    direction = normalize(direction);
    direction.w = 0.0f;

    velocity = direction * speed;
    velocity.w = 0.0f;

    lifeTime = 2.0f;

    scale = { 0.12f, 0.12f, 0.12f, 0.0f };

    tipo = OBJ_BULLET;
    active = true;

    updateModelMatrix();

    std::cout << "[Bullet] Bala inicializada correctamente\n";
    std::cout << "[Bullet] Posicion: "
        << position.x << ", "
        << position.y << ", "
        << position.z << "\n";

    std::cout << "[Bullet] Velocity: "
        << velocity.x << ", "
        << velocity.y << ", "
        << velocity.z << "\n";
}

void Bullet::move(double timeStep)
{
    float dt = static_cast<float>(timeStep);

    // Movimiento rectilíneo:
    // posicion = posicion + velocidad * tiempo
    position = position + velocity * dt;
    position.w = 1.0f;

    lifeTime -= dt;

    if (lifeTime <= 0.0f)
    {
        active = false;
    }

    // Límite amplio para borrar balas lejanas.
    if (position.x < -50.0f || position.x > 50.0f ||
        position.y < -50.0f || position.y > 50.0f ||
        position.z < -50.0f || position.z > 50.0f)
    {
        active = false;
    }

    updateModelMatrix();
}