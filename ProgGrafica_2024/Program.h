#pragma once
#include "common.h"
#include "libMath.h"
#include "Shader.h"

class Camera;
class Luz;
class Material;

#include <vector>
#include <map>
#include <string>

enum dataType_e
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_VEC4,
    TYPE_MAT4
};

class Program
{
public:
    // Identificador del programa OpenGL
    unsigned int idProgram;

    // Lista de shaders que forman el programa
    std::vector<Shader*> shaderList;

    // Mapa nombre de variable -> location en GPU
    std::map<std::string, int> varList;

    Program();
    ~Program();

    // Construccion del programa
    void addShader(std::string fileName);
    void linkProgram();
    void checkErrors();
    void clean();
    void readVarList();

    // Uso del programa
    void use();

    // Setters genericos
    int getLocation(std::string nombre);

    void setUniformData(dataType_e tipo, void* dato, std::string nombre);

    void setAttributeData(
        std::string nombre,
        GLint size,
        GLenum type,
        GLboolean normalized,
        GLsizei stride,
        const GLvoid* pointer
    );

    // Setters especificos para PR5
    void setMatrix(matrix4x4f matrix, std::string nombre);
    void setCamera(Camera* cam);
    void setLight(Luz* luz);
    void setMaterial(Material* mat);
};