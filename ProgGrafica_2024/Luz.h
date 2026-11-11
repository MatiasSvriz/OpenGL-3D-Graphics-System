#pragma once
#include "libMath.h"

class Luz
{
public:

    // Posicion de la luz en coordenadas de mundo.
    // w = 1 porque representa un punto.
    vec4float posicion;

    // Color de la luz.
    // Para luz blanca: {1, 1, 1, 1}
    vec4float color;

    // Intensidades de la luz
    float Ia; // Ambiental
    float Id; // Difusa
    float Is; // Especular

    // Estado de la luz
    bool activa;

    // Constructores
    Luz();

    Luz(vec4float posicion, vec4float color, float Ia, float Id, float Is);
};