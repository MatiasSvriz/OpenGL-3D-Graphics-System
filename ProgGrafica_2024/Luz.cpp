#include "Luz.h"

Luz::Luz()
{
    posicion = { 0.0f, 0.0f, 0.0f, 1.0f };
    color = { 1.0f, 1.0f, 1.0f, 1.0f };

    Ia = 0.2f;
    Id = 0.8f;
    Is = 1.0f;

    activa = true;
}

Luz::Luz(vec4float posicion, vec4float color, float Ia, float Id, float Is)
{
    this->posicion = posicion;
    this->color = color;

    this->Ia = Ia;
    this->Id = Id;
    this->Is = Is;

    this->activa = true;
}