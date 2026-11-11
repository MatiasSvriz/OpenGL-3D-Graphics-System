#include "Shader.h"

Shader::Shader(std::string fileName)
{
    this->idShader = 0;
    this->fileName = fileName;
    this->type = 0;
    this->source = "";
    this->compiled = false;

    detectType();
    readSource();
    compileShader();
}

Shader::~Shader()
{
}

void Shader::detectType()
{
    if (fileName.ends_with(".vertex"))
    {
        type = GL_VERTEX_SHADER;
    }
    else if (fileName.ends_with(".fragment"))
    {
        type = GL_FRAGMENT_SHADER;
    }
    else
    {
        type = 0;
        std::cout << "ERROR: tipo de shader no reconocido en " << fileName << "\n";
    }
}

void Shader::readSource()
{
    std::ifstream f(fileName);

    if (f.is_open())
    {
        source = std::string(std::istreambuf_iterator<char>(f), {});
    }
    else
    {
        std::cout << "ERROR: FICHERO NO ENCONTRADO "  << __FILE__ << ":" << __LINE__ << " " << fileName << "\n";
    }
}

void Shader::compileShader()
{
    if (type == 0)
    {
        std::cout << "ERROR: no se puede compilar shader sin tipo válido: "
            << fileName << "\n";
        return;
    }

    if (source.empty())
    {
        std::cout << "ERROR: código vacío en shader " << fileName << "\n";
        return;
    }

    idShader = glCreateShader(type);

    const char* src = source.c_str();
    glShaderSource(idShader, 1, &src, nullptr);
    glCompileShader(idShader);

    checkErrors();
}

void Shader::checkErrors()
{
    GLint compiledOK = GL_FALSE;
    glGetShaderiv(idShader, GL_COMPILE_STATUS, &compiledOK);

    if (compiledOK != GL_TRUE)
    {
        GLsizei logLength = 0;
        GLchar message[1024];

        glGetShaderInfoLog(idShader, 1024, &logLength, message);

        std::cout << "ERROR compilando shader: " << fileName << "\n";
        std::cout << message << "\n\n";

        compiled = false;
    }
    else
    {
        compiled = true;
        std::cout << "Shader compilado correctamente: " << fileName << "\n";
    }
}

void Shader::clean()
{
    if (idShader != 0)
    {
        glDeleteShader(idShader);
        idShader = 0;
    }

    source.clear();
    compiled = false;
}
