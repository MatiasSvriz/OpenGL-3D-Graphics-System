#pragma once
#include "libMath.h"
#include "Colliders.h"

class Camera
{
public:
    // Posicion y rotacion de la camara
    vec4float pos;  // posicion XYZ en el mundo
    vec4float rot;  // rotacion en radianes (X = pitch, Y = yaw)

    // Vectores de vista
    vec4float lookAt;   // punto al que mira
    vec4float up;       // vector "arriba"

    // Parametros de proyeccion perspectiva
    float fovy;         // angulo de apertura vertical (radianes)
    float aspectRatio;  // ancho / alto
    float zNear;
    float zFar;

    // Sensibilidades de movimiento
    float moveSpeed = 3.0f;   // unidades/s con flechas
    float mouseSensX = 0.003f; // radianes/pixel horizontal
    float mouseSensY = 0.003f; // radianes/pixel vertical

    Collider* coll = nullptr;

    Camera(vec4float pos, vec4float rot,
        vec4float lookAt, vec4float up,
        float fovy, float aspectRatio,
        float zNear, float zFar);

    // Calcula la matriz de vista (LookAt)
    matrix4x4f getMatrixLookAt();

    // Calcula la matriz de proyeccion perspectiva
    matrix4x4f getMatrixPerspective();

    // Actualiza posicion/rotacion segun input de teclado y raton
    void move(double timeStep);

    // PR6
    void createCollider();
    void updateCollider();

};