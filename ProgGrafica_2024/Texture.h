#pragma once
#include "common.h"

#include <string>
#include <vector>

// Representa un pixel RGBA de 8 bits por canal.
typedef struct pixel_t
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

} pixel_t;

class Texture
{
public:

    // Nombre del fichero de textura
    std::string textureName;

    // Identificador de textura en OpenGL
    unsigned int GLId;

    // Tamaño de la imagen
    int w;
    int h;

    // Datos de la imagen en CPU
    std::vector<pixel_t> rawImage;

    // Indica si la textura se ha cargado correctamente
    bool loaded;

    // Constructores / destructor
    Texture();

    Texture(int w, int h, pixel_t color);

    Texture(std::string fileName);

    ~Texture();

    // Copia los datos de rawImage a GPU
    void updateTexture();

    // Activa esta textura en una unidad de textura concreta
    void bind(unsigned int textureUnit = 0);
};
