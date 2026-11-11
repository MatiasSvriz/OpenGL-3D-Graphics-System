#include "Object3D.h"
#include "InputManager.h"
#include "Material.h"
#include "Texture.h"
#include "Colliders.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

bool readDataLine(std::ifstream& file, std::string& out)
{
    while (std::getline(file, out))
    {
        if (out.size() >= 3 &&
            (unsigned char)out[0] == 0xEF &&
            (unsigned char)out[1] == 0xBB &&
            (unsigned char)out[2] == 0xBF)
        {
            out = out.substr(3);
        }

        if (!out.empty() && out.back() == '\r')
            out.pop_back();

        size_t first = out.find_first_not_of(" \t");

        if (first == std::string::npos)
            continue;

        out = out.substr(first);

        if (out[0] == '#')
            continue;

        if ((unsigned char)out[0] == 0xE2)
            continue;

        return true;
    }

    return false;
}

std::vector<float> readMultipleData(const std::string& line)
{
    std::vector<float> data;
    std::istringstream lineReader(line);
    std::string token;

    while (lineReader >> token)
    {
        if (!token.empty() && (token.back() == 'f' || token.back() == 'F'))
            token.pop_back();

        data.push_back(std::stof(token));
    }

    return data;
}

Object3D::Object3D()
{
    objectId = idCounter++;

    prg = nullptr;
    mat = nullptr;
    coll = nullptr;

    position = { 0, 0, 0, 1 };
    rotation = { 0, 0, 0, 0 };
    scale = { 1, 1, 1, 0 };

    modelMatrix = make_identityf();

    tipo = OBJ_GENERIC;
    active = true;
}

Object3D::~Object3D()
{
    if (coll != nullptr)
    {
        for (particle3D* particle : coll->particleList)
            delete particle;

        coll->particleList.clear();

        delete coll;
        coll = nullptr;
    }

    if (prg != nullptr)
    {
        delete prg;
        prg = nullptr;
    }

    if (mat != nullptr)
    {
        delete mat;
        mat = nullptr;
    }
}

void Object3D::createTriangle()
{
    vertexList.clear();
    idList.clear();

    vertexList = {
        {
            { 0.0f, 0.5f, 0.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f, 0.0f },
            { 0.5f, 1.0f, 0.0f, 0.0f }
        },
        {
            { -0.5f, -0.5f, 0.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f }
        },
        {
            { 0.5f, -0.5f, 0.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f, 1.0f },
            { 0.0f, 0.0f, 1.0f, 0.0f },
            { 1.0f, 0.0f, 0.0f, 0.0f }
        }
    };

    idList = { 0, 1, 2 };

    updateModelMatrix();
    createSphereCollider();
}

void Object3D::computeNormals()
{
    for (Vertex& v : vertexList)
        v.normal = { 0, 0, 0, 0 };

    for (size_t i = 0; i + 2 < idList.size(); i += 3)
    {
        unsigned int i0 = idList[i];
        unsigned int i1 = idList[i + 1];
        unsigned int i2 = idList[i + 2];

        if (i0 >= vertexList.size() || i1 >= vertexList.size() || i2 >= vertexList.size())
            continue;

        vec4float p0 = vertexList[i0].position;
        vec4float p1 = vertexList[i1].position;
        vec4float p2 = vertexList[i2].position;

        vec4float edge1 = p1 - p0;
        vec4float edge2 = p2 - p0;

        vec4float normal = normalize(cross(edge1, edge2));
        normal.w = 0.0f;

        vertexList[i0].normal = vertexList[i0].normal + normal;
        vertexList[i1].normal = vertexList[i1].normal + normal;
        vertexList[i2].normal = vertexList[i2].normal + normal;
    }

    for (Vertex& v : vertexList)
    {
        v.normal = normalize(v.normal);
        v.normal.w = 0.0f;
    }
}

bool Object3D::loadFromFile(const char* fileName)
{
    enum dataTypesReading
    {
        vertices,
        faces,
        colors,
        normals,
        textureCoords,
        done
    };

    std::ifstream f(fileName);

    if (!f.is_open())
    {
        std::cout << "ERROR: no se pudo abrir el fichero FRS: " << fileName << "\n";
        return false;
    }

    vertexList.clear();
    idList.clear();

    std::string textureFile;
    std::string vertexShader;
    std::string fragmentShader;
    std::string line;
    std::istringstream lineReader;

    dataTypesReading mode = vertices;

    bool normalsLoaded = false;

    while (!f.eof() && mode != done)
    {
        switch (mode)
        {
        case vertices:
        {
            int numVertex = 0;

            if (!readDataLine(f, line))
                return false;

            lineReader = std::istringstream(line);
            lineReader >> numVertex;

            vertexList.resize(numVertex);

            for (int i = 0; i < numVertex; i++)
            {
                if (!readDataLine(f, line))
                    return false;

                lineReader = std::istringstream(line);

                lineReader >> vertexList[i].position.x;
                lineReader >> vertexList[i].position.y;
                lineReader >> vertexList[i].position.z;

                vertexList[i].position.w = 1.0f;
                vertexList[i].color = { 1, 1, 1, 1 };
                vertexList[i].normal = { 0, 0, 0, 0 };
                vertexList[i].textureCoords = { 0, 0, 0, 0 };
            }

            mode = faces;
            break;
        }

        case faces:
        {
            int numFaces = 0;

            if (!readDataLine(f, line))
                return false;

            lineReader = std::istringstream(line);
            lineReader >> numFaces;

            idList.resize(numFaces * 3);

            for (int i = 0; i < numFaces; i++)
            {
                if (!readDataLine(f, line))
                    return false;

                lineReader = std::istringstream(line);

                lineReader >> idList[i * 3];
                lineReader >> idList[i * 3 + 1];
                lineReader >> idList[i * 3 + 2];
            }

            mode = colors;
            break;
        }

        case colors:
        {
            int numColors = 0;

            if (!readDataLine(f, line))
                return false;

            lineReader = std::istringstream(line);
            lineReader >> numColors;

            for (int i = 0; i < numColors; i++)
            {
                if (!readDataLine(f, line))
                    return false;

                std::vector<float> data = readMultipleData(line);

                if (data.size() < 5)
                {
                    std::cout << "ERROR: linea de color mal formada\n";
                    return false;
                }

                int numVertexIds = static_cast<int>(data.size()) - 4;

                vec4float color = {
                    data[numVertexIds],
                    data[numVertexIds + 1],
                    data[numVertexIds + 2],
                    data[numVertexIds + 3]
                };

                for (int j = 0; j < numVertexIds; j++)
                {
                    unsigned int id = static_cast<unsigned int>(data[j]);

                    if (id < vertexList.size())
                        vertexList[id].color = color;
                }
            }

            mode = normals;
            break;
        }

        case normals:
        {
            int numNormals = 0;

            if (!readDataLine(f, line))
                return false;

            lineReader = std::istringstream(line);
            lineReader >> numNormals;

            for (int i = 0; i < numNormals; i++)
            {
                if (!readDataLine(f, line))
                    return false;

                std::vector<float> data = readMultipleData(line);

                if (data.size() < 5)
                {
                    std::cout << "ERROR: linea de normal mal formada\n";
                    return false;
                }

                int numVertexIds = static_cast<int>(data.size()) - 4;

                vec4float normal = {
                    data[numVertexIds],
                    data[numVertexIds + 1],
                    data[numVertexIds + 2],
                    data[numVertexIds + 3]
                };

                normal = normalize(normal);
                normal.w = 0.0f;

                for (int j = 0; j < numVertexIds; j++)
                {
                    unsigned int id = static_cast<unsigned int>(data[j]);

                    if (id < vertexList.size())
                    {
                        vertexList[id].normal = normal;
                        normalsLoaded = true;
                    }
                }
            }

            if (!normalsLoaded)
                computeNormals();

            mode = textureCoords;
            break;
        }

        case textureCoords:
        {
            int numTC = 0;

            if (!readDataLine(f, line))
                return false;

            lineReader = std::istringstream(line);
            lineReader >> numTC;

            for (int i = 0; i < numTC; i++)
            {
                if (!readDataLine(f, line))
                    return false;

                std::vector<float> data = readMultipleData(line);

                if (data.size() < 3)
                {
                    std::cout << "ERROR: linea de coordenada de textura mal formada\n";
                    return false;
                }

                int numVertexIds = static_cast<int>(data.size()) - 2;

                float u = data[numVertexIds];
                float v = data[numVertexIds + 1];

                for (int j = 0; j < numVertexIds; j++)
                {
                    unsigned int id = static_cast<unsigned int>(data[j]);

                    if (id < vertexList.size())
                    {
                        vertexList[id].textureCoords.x = u;
                        vertexList[id].textureCoords.y = v;
                        vertexList[id].textureCoords.z = 0.0f;
                        vertexList[id].textureCoords.w = 0.0f;
                    }
                }
            }

            if (!readDataLine(f, textureFile))
            {
                std::cout << "ERROR: falta fichero de textura en el FRS\n";
                return false;
            }

            if (!readDataLine(f, vertexShader))
            {
                std::cout << "ERROR: falta vertex shader en el FRS\n";
                return false;
            }

            if (!readDataLine(f, fragmentShader))
            {
                std::cout << "ERROR: falta fragment shader en el FRS\n";
                return false;
            }

            mode = done;
            break;
        }

        case done:
            break;
        }
    }

    if (prg != nullptr)
    {
        delete prg;
        prg = nullptr;
    }

    prg = new Program();
    prg->addShader(vertexShader);
    prg->addShader(fragmentShader);
    prg->linkProgram();

    if (mat != nullptr)
    {
        delete mat;
        mat = nullptr;
    }

    mat = new Material();

    mat->ka = 0.2f;
    mat->kd = 0.8f;
    mat->ks = 0.5f;
    mat->shiny = 32;
    mat->alpha = 1.0f;
    mat->color = { 1, 1, 1, 1 };

    if (!textureFile.empty())
    {
        mat->textura = new Texture(textureFile);
        mat->usaTextura = true;
    }
    else
    {
        mat->usaTextura = false;
    }

    updateModelMatrix();
    createSphereCollider();

    return true;
}

void Object3D::move(double timeStep)
{
    float rotationSpeed = 1.0f;

    if (InputManager::keyState[GLFW_KEY_A])
        rotation.y -= rotationSpeed * static_cast<float>(timeStep);

    if (InputManager::keyState[GLFW_KEY_D])
        rotation.y += rotationSpeed * static_cast<float>(timeStep);

    updateModelMatrix();
}

void Object3D::updateModelMatrix()
{
    matrix4x4f T = make_translate(position.x, position.y, position.z);
    matrix4x4f R = make_rotate(rotation.x, rotation.y, rotation.z);
    matrix4x4f S = make_scale(scale.x, scale.y, scale.z);

    modelMatrix = T * R * S;

    updateCollider();
}

void Object3D::createSphereCollider()
{
    if (vertexList.empty() || idList.empty())
        return;

    if (coll != nullptr)
    {
        for (particle3D* particle : coll->particleList)
            delete particle;

        coll->particleList.clear();

        delete coll;
        coll = nullptr;
    }

    Sphere* sphere = new Sphere();

    // Cada triángulo genera una partícula para construir la esfera envolvente.
    for (size_t i = 0; i + 2 < idList.size(); i += 3)
    {
        unsigned int i0 = idList[i];
        unsigned int i1 = idList[i + 1];
        unsigned int i2 = idList[i + 2];

        if (i0 >= vertexList.size() || i1 >= vertexList.size() || i2 >= vertexList.size())
            continue;

        vec4float p0 = vertexList[i0].position;
        vec4float p1 = vertexList[i1].position;
        vec4float p2 = vertexList[i2].position;

        float minX = p0.x;
        float minY = p0.y;
        float minZ = p0.z;

        if (p1.x < minX) minX = p1.x;
        if (p2.x < minX) minX = p2.x;

        if (p1.y < minY) minY = p1.y;
        if (p2.y < minY) minY = p2.y;

        if (p1.z < minZ) minZ = p1.z;
        if (p2.z < minZ) minZ = p2.z;

        float maxX = p0.x;
        float maxY = p0.y;
        float maxZ = p0.z;

        if (p1.x > maxX) maxX = p1.x;
        if (p2.x > maxX) maxX = p2.x;

        if (p1.y > maxY) maxY = p1.y;
        if (p2.y > maxY) maxY = p2.y;

        if (p1.z > maxZ) maxZ = p1.z;
        if (p2.z > maxZ) maxZ = p2.z;

        particle3D* particle = new particle3D();

        particle->pos = {
            (minX + maxX) / 2.0f,
            (minY + maxY) / 2.0f,
            (minZ + maxZ) / 2.0f,
            1.0f
        };

        particle->size = {
            maxX - minX,
            maxY - minY,
            maxZ - minZ,
            0.0f
        };

        sphere->addParticle(particle);
    }

    coll = sphere;

    updateCollider();
}

void Object3D::updateCollider()
{
    if (coll != nullptr)
        coll->updateCollider(modelMatrix);
}