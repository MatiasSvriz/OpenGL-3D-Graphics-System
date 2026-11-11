#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cstring>
#include <iostream>

Texture::Texture()
{
    textureName = "";
    GLId = 0;
    w = 0;
    h = 0;
    loaded = false;
}

Texture::Texture(int w, int h, pixel_t color)
{
    textureName = "";
    this->w = w;
    this->h = h;
    GLId = 0;
    loaded = false;

    rawImage.resize(w * h);

    for (pixel_t& pixel : rawImage)
        pixel = color;

    glGenTextures(1, &GLId);

    loaded = true;

    updateTexture();
}

Texture::Texture(std::string fileName)
{
    textureName = fileName;
    GLId = 0;
    w = 0;
    h = 0;
    loaded = false;

    int numChannels = 0;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* pixels = stbi_load(
        fileName.c_str(),
        &w,
        &h,
        &numChannels,
        4
    );

    if (pixels == nullptr)
    {
        std::cout << "ERROR: no se pudo cargar la textura: " << fileName << "\n";
        return;
    }

    rawImage.resize(w * h);

    std::memcpy(
        rawImage.data(),
        pixels,
        sizeof(pixel_t) * w * h
    );

    stbi_image_free(pixels);

    glGenTextures(1, &GLId);

    loaded = true;

    updateTexture();
}

Texture::~Texture()
{
    if (GLId != 0)
    {
        glDeleteTextures(1, &GLId);
        GLId = 0;
    }

    rawImage.clear();
    loaded = false;
}

void Texture::updateTexture()
{
    if (!loaded || GLId == 0 || rawImage.empty())
        return;

    glBindTexture(GL_TEXTURE_2D, GLId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        w,
        h,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        rawImage.data()
    );

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(unsigned int textureUnit)
{
    if (!loaded || GLId == 0)
        return;

    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, GLId);
}