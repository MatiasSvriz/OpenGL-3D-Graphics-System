#include "Material.h"

#include <iostream>

Material::Material()
{
    ka = 0.2f;
    kd = 0.8f;
    ks = 0.5f;

    usaTextura = false;
    textura = nullptr;

    color = { 1.0f, 1.0f, 1.0f, 1.0f };

    shiny = 32;
    alpha = 1.0f;
}

Material::Material(std::string textureName, float ka, float kd, float ks)
{
    this->ka = ka;
    this->kd = kd;
    this->ks = ks;

    usaTextura = false;
    textura = nullptr;

    color = { 1.0f, 1.0f, 1.0f, 1.0f };

    shiny = 32;
    alpha = 1.0f;

    setTexture(textureName);
}

Material::~Material()
{
    clearTexture();
}

void Material::setTexture(std::string textureName)
{
    clearTexture();

    if (textureName.empty())
        return;

    textura = new Texture(textureName);

    if (textura->loaded)
    {
        usaTextura = true;
    }
    else
    {
        delete textura;
        textura = nullptr;
        usaTextura = false;

        std::cout << "ERROR: no se pudo cargar la textura: " << textureName << "\n";
    }
}

void Material::clearTexture()
{
    if (textura != nullptr)
    {
        delete textura;
        textura = nullptr;
    }

    usaTextura = false;
}