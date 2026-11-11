#pragma once

#include "Object3D.h"

class Planet : public Object3D
{
public:
    int health = 3;

    bool orbit = false;

    vec4float orbitCenter = { 0.0f, 0.0f, 0.0f, 1.0f };
    float orbitRadius = 1.0f;
    float orbitAngle = 0.0f;
    float orbitSpeed = 0.5f;

    Planet();

    void init(
        const char* frsFile,
        vec4float startPosition,
        float size
    );

    void hit();

    virtual void move(double timeStep) override;
};