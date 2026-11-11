#pragma once
#include "common.h"
#include <string>

class Shader
{
public:
    unsigned int idShader;   // identificador OpenGL del shader
    std::string fileName;    // nombre del fichero GLSL
    GLenum type;             // GL_VERTEX_SHADER o GL_FRAGMENT_SHADER
    std::string source;      // código fuente del shader
    bool compiled;           // indica si ha compilado correctamente

    Shader(std::string fileName);
    ~Shader();

    void readSource();
    void compileShader();
    void checkErrors();
    void clean();

private:
    void detectType();
};

