#include "Program.h"

#include "Camera.h"
#include "Luz.h"
#include "Material.h"
#include "Texture.h"

#include <iostream>

// ---------------------------------------------------------
// Constructor
// ---------------------------------------------------------

Program::Program()
{
    idProgram = glCreateProgram();
}


// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------

Program::~Program()
{
    clean();
}


// ---------------------------------------------------------
// addShader
// ---------------------------------------------------------
// Crea un shader individual y lo guarda en shaderList.
// El constructor de Shader se encarga de leer y compilar.
// ---------------------------------------------------------

void Program::addShader(std::string fileName)
{
    Shader* sh = new Shader(fileName);
    shaderList.push_back(sh);
}


// ---------------------------------------------------------
// linkProgram
// ---------------------------------------------------------
// Une todos los shaders en un unico programa ejecutable.
// ---------------------------------------------------------

void Program::linkProgram()
{
    for (auto* sh : shaderList)
    {
        if (sh != nullptr && sh->idShader != 0)
        {
            glAttachShader(idProgram, sh->idShader);
        }
    }

    glLinkProgram(idProgram);

    checkErrors();

    // Importante:
    // Leemos las variables activas despues de linkar.
    readVarList();

    // Una vez linkado, los shaders individuales se pueden borrar.
    for (auto* sh : shaderList)
    {
        if (sh != nullptr)
        {
            glDetachShader(idProgram, sh->idShader);
            sh->clean();
            delete sh;
        }
    }

    shaderList.clear();
}


// ---------------------------------------------------------
// checkErrors
// ---------------------------------------------------------
// Comprueba errores de linkado.
// ---------------------------------------------------------

void Program::checkErrors()
{
    GLint linked = GL_FALSE;
    glGetProgramiv(idProgram, GL_LINK_STATUS, &linked);

    if (linked != GL_TRUE)
    {
        GLsizei logLength = 0;
        GLchar message[1024];

        glGetProgramInfoLog(idProgram, 1024, &logLength, message);

        std::cout << "ERROR linkando programa:\n";
        std::cout << message << "\n\n";
    }
}


// ---------------------------------------------------------
// clean
// ---------------------------------------------------------
// Libera programa de GPU y shaders pendientes.
// ---------------------------------------------------------

void Program::clean()
{
    for (auto* sh : shaderList)
    {
        if (sh != nullptr)
        {
            sh->clean();
            delete sh;
        }
    }

    shaderList.clear();

    if (idProgram != 0)
    {
        glDeleteProgram(idProgram);
        idProgram = 0;
    }

    varList.clear();
}


// ---------------------------------------------------------
// readVarList
// ---------------------------------------------------------
// Interroga al programa linkado para saber que atributos y
// uniforms existen realmente.
// ---------------------------------------------------------

void Program::readVarList()
{
    varList.clear();

    GLint numAttributes = 0;
    GLint numUniforms = 0;

    // -----------------------------
    // Atributos
    // -----------------------------
    glGetProgramiv(idProgram, GL_ACTIVE_ATTRIBUTES, &numAttributes);

    for (int i = 0; i < numAttributes; i++)
    {
        char varName[128];
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = 0;

        glGetActiveAttrib(
            idProgram,
            static_cast<GLuint>(i),
            128,
            &length,
            &size,
            &type,
            varName
        );

        std::string name(varName);
        varList[name] = glGetAttribLocation(idProgram, name.c_str());
    }

    // -----------------------------
    // Uniforms
    // -----------------------------
    glGetProgramiv(idProgram, GL_ACTIVE_UNIFORMS, &numUniforms);

    for (int i = 0; i < numUniforms; i++)
    {
        char varName[128];
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = 0;

        glGetActiveUniform(
            idProgram,
            static_cast<GLuint>(i),
            128,
            &length,
            &size,
            &type,
            varName
        );

        std::string name(varName);

        // Si el uniform es un array, OpenGL puede devolver "array[0]".
        // Registramos tanto el nombre base como cada elemento.
        if (!name.empty() && name.back() == ']')
        {
            std::string baseName = name.substr(0, name.find('['));

            varList[baseName] = glGetUniformLocation(idProgram, baseName.c_str());

            for (int j = 0; j < size; j++)
            {
                std::string indexedName = baseName + "[" + std::to_string(j) + "]";
                varList[indexedName] = glGetUniformLocation(idProgram, indexedName.c_str());
            }
        }
        else
        {
            varList[name] = glGetUniformLocation(idProgram, name.c_str());
        }
    }
}


// ---------------------------------------------------------
// use
// ---------------------------------------------------------

void Program::use()
{
    glUseProgram(idProgram);
}


// ---------------------------------------------------------
// getLocation
// ---------------------------------------------------------
// Devuelve la localizacion de una variable.
// Si no estaba en varList, intenta buscarla directamente.
// Esto es util con uniforms de structs como:
// luz.pos
// material.ka
// camara.posicion
// ---------------------------------------------------------

int Program::getLocation(std::string nombre)
{
    auto it = varList.find(nombre);

    if (it != varList.end())
        return it->second;

    // Intento como uniform
    GLint uniformLocation = glGetUniformLocation(idProgram, nombre.c_str());

    if (uniformLocation != -1)
    {
        varList[nombre] = uniformLocation;
        return uniformLocation;
    }

    // Intento como atributo
    GLint attribLocation = glGetAttribLocation(idProgram, nombre.c_str());

    if (attribLocation != -1)
    {
        varList[nombre] = attribLocation;
        return attribLocation;
    }

    return -1;
}


// ---------------------------------------------------------
// setUniformData
// ---------------------------------------------------------

void Program::setUniformData(dataType_e tipo, void* dato, std::string nombre)
{
    GLint location = getLocation(nombre);

    if (location == -1)
    {
        // No lo tratamos como error fatal.
        // GLSL puede eliminar uniforms no usados.
        std::cout << "WARNING: uniform no encontrado o no usado: "
            << nombre << "\n";
        return;
    }

    switch (tipo)
    {
    case TYPE_INT:
        glUniform1i(location, *(int*)dato);
        break;

    case TYPE_FLOAT:
        glUniform1f(location, *(float*)dato);
        break;

    case TYPE_VEC4:
    {
        vec4float* v = (vec4float*)dato;
        glUniform4f(location, v->x, v->y, v->z, v->w);
        break;
    }

    case TYPE_MAT4:
    {
        matrix4x4f* m = (matrix4x4f*)dato;

        // Tu libreria trabaja con matrices en orden de filas.
        // En las practicas anteriores normalmente estabamos pasando
        // la matriz transpuesta antes de llegar aqui.
        // Por eso dejamos GL_FALSE.
        glUniformMatrix4fv(location, 1, GL_FALSE, m->mat);
        break;
    }

    default:
        std::cout << "ERROR: tipo de uniform no soportado: "
            << nombre << "\n";
        break;
    }
}


// ---------------------------------------------------------
// setAttributeData
// ---------------------------------------------------------

void Program::setAttributeData(
    std::string nombre,
    GLint size,
    GLenum type,
    GLboolean normalized,
    GLsizei stride,
    const GLvoid* pointer
)
{
    GLint location = getLocation(nombre);

    if (location == -1)
    {
        std::cout << "WARNING: atributo no encontrado o no usado: "
            << nombre << "\n";
        return;
    }

    glEnableVertexAttribArray(static_cast<GLuint>(location));

    glVertexAttribPointer(
        static_cast<GLuint>(location),
        size,
        type,
        normalized,
        stride,
        pointer
    );
}


// ---------------------------------------------------------
// setMatrix
// ---------------------------------------------------------
// Setter comodo para matrices tipo MVP o M.
// ---------------------------------------------------------

void Program::setMatrix(matrix4x4f matrix, std::string nombre)
{
    setUniformData(TYPE_MAT4, &matrix, nombre);
}


// ---------------------------------------------------------
// setCamera
// ---------------------------------------------------------
// Envia los datos de la camara al shader.
// El fragment shader usa la posicion de la camara para calcular
// el vector de vista en la componente especular.
// ---------------------------------------------------------

void Program::setCamera(Camera* cam)
{
    if (cam == nullptr)
        return;

    setUniformData(TYPE_VEC4, &cam->pos, "camara.posicion");
}


// ---------------------------------------------------------
// setLight
// ---------------------------------------------------------
// Envia la luz puntual al shader.
// ---------------------------------------------------------

void Program::setLight(Luz* luz)
{
    if (luz == nullptr)
        return;

    setUniformData(TYPE_VEC4, &luz->posicion, "luz.pos");
    setUniformData(TYPE_VEC4, &luz->color, "luz.color");
    setUniformData(TYPE_FLOAT, &luz->Ia, "luz.Ia");
    setUniformData(TYPE_FLOAT, &luz->Id, "luz.Id");
    setUniformData(TYPE_FLOAT, &luz->Is, "luz.Is");

    int activa = luz->activa ? 1 : 0;
    setUniformData(TYPE_INT, &activa, "luz.activa");
}


// ---------------------------------------------------------
// setMaterial
// ---------------------------------------------------------
// Envia el material al shader y activa la textura si existe.
// ---------------------------------------------------------

void Program::setMaterial(Material* mat)
{
    if (mat == nullptr)
        return;

    setUniformData(TYPE_FLOAT, &mat->ka, "material.ka");
    setUniformData(TYPE_FLOAT, &mat->kd, "material.kd");
    setUniformData(TYPE_FLOAT, &mat->ks, "material.ks");

    int usaTextura = mat->usaTextura ? 1 : 0;
    setUniformData(TYPE_INT, &usaTextura, "material.usaTextura");

    setUniformData(TYPE_VEC4, &mat->color, "material.color");

    int shiny = mat->shiny;
    setUniformData(TYPE_INT, &shiny, "material.shiny");

    setUniformData(TYPE_FLOAT, &mat->alpha, "material.alpha");

    // Textura de color
    if (mat->usaTextura && mat->textura != nullptr && mat->textura->loaded)
    {
        mat->textura->bind(0);

        int textureUnit = 0;
        setUniformData(TYPE_INT, &textureUnit, "colorTexture");
    }
}