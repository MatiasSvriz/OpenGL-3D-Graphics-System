#pragma once
#include "libMath.h"
#include "Program.h"
#include "Material.h"
#include "Colliders.h"
#include "common.h"

#include <vector>
#include <string>

// Nos permite distinguir objetos dentro del sistema de colisiones.
enum ObjectType
{
    OBJ_GENERIC = 0,
    OBJ_CUBE = 1,
    OBJ_WALL = 2,
    OBJ_ENEMY = 3,

    // Solar Shooter POV
    OBJ_BULLET = 10,
    OBJ_PLANET = 11,
    OBJ_STAR = 12,
    OBJ_EXPLOSION = 13,
    OBJ_SUN = 14
};

typedef struct Vertex
{
    vec4float position;       // Posicion del vertice en coordenadas de modelo
    vec4float color;          // Color base del vertice
    vec4float normal;         // Normal del vertice para iluminacion
    vec4float textureCoords;  // Coordenadas de textura. Usaremos x,y como u,v

} Vertex;

class Object3D
{
public:

    // Identificador unico por objeto
    // Sirve para asociar este objeto con sus buffers en Render.
    static inline unsigned int idCounter = 0;
    unsigned int objectId = 0;

    // Programa de GPU usado por este objeto.
    Program* prg = nullptr;

    // Material del objeto.
    // Contiene textura y constantes de iluminacion:
    // Ka, Kd, Ks, shiny, alpha, etc.
    Material* mat = nullptr;

    // Transformacion del objeto
    vec4float position = { 0, 0, 0, 1 };
    vec4float rotation = { 0, 0, 0, 0 }; // angulos en radianes
    vec4float scale = { 1, 1, 1, 0 };

    matrix4x4f modelMatrix = make_identityf();

    // Geometria
    std::vector<Vertex> vertexList;
    std::vector<unsigned int> idList;

    // Colisionador asociado al objeto.
    // En esta práctica será normalmente una Sphere.
    Collider* coll = nullptr;

    // Tipo de objeto. Sirve para filtrar colisiones.
    int tipo = OBJ_GENERIC;

    // Si active == false, Render podrá eliminar este objeto.
    bool active = true;

    // Constructores / destructor
    Object3D();
    virtual ~Object3D();

    void createTriangle();

    // Carga desde fichero FRS
    bool loadFromFile(const char* fileName);

    // Movimiento del objeto.
    // Virtual para que Cubo pueda sobrescribirlo y orbitar.
    virtual void move(double timeStep);

    // Actualizar matriz modelo:
    // modelo = traslacion * rotacion * escala
    void updateModelMatrix();

    // Calcular normales automaticamente si el fichero no las trae.
    void computeNormals();

    // PR6: FUNCIONES DE COLISIÓN
    
    // Crea una esfera envolvente a partir de los triángulos del objeto.
    void createSphereCollider();

    // Actualiza el collider usando la matriz modelo actual.
    void updateCollider();
};