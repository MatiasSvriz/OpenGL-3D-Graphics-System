#pragma once

#include "Object3D.h"

class Bullet : public Object3D
{
public:
    // Velocidad de la bala en unidades por segundo.
    vec4float velocity = { 0.0f, 0.0f, 0.0f, 0.0f };

    // Tiempo máximo de vida.
    float lifeTime = 2.0f;

    Bullet();

    void init(
        const char* frsFile,
        vec4float startPosition,
        vec4float direction,
        float speed
    );

    virtual void move(double timeStep) override;
};