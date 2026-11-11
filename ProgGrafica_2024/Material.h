#pragma once

#include "common.h"
#include "libMath.h"
#include "Texture.h"

#include <string>

class Material
{
public:

    // ka: cuanto responde el material a la luz ambiental
    // kd: cuanto responde el material a la luz difusa
    // ks: cuanto responde el material a la luz especular
    float ka;
    float kd;
    float ks;

    // Textura
    bool usaTextura;
    Texture* textura;

    // Color base del material
    // Se usa cuando no hay textura.
    vec4float color;

    // Brillo especular
    // Valores bajos: brillo amplio y mate
    // Valores altos: brillo pequeno y concentrado
    int shiny;

    // Transparencia/opacidad
    // 1.0 -> totalmente opaco
    // 0.0 -> totalmente transparente
    float alpha;

    // Constructores / destructor-
    Material();

    Material(std::string textureName, float ka, float kd, float ks);

    ~Material();

    // Cargar o cambiar textura
    void setTexture(std::string textureName);

    // Eliminar textura actual
    void clearTexture();
};

