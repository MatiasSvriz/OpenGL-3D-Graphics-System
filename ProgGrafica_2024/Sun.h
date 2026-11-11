#pragma once

#include "Object3D.h"

class Sun : public Object3D
{
public:
    Sun();

    void init(
        const char* frsFile,
        vec4float startPosition,
        float size
    );

    virtual void move(double timeStep) override;
};