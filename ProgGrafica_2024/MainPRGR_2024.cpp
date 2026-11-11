#define GLAD_BIN

#include "Render.h"
#include "Camera.h"
#include "Luz.h"
#include "Planet.h"
#include "Sun.h"

#include <iostream>
#include <cmath>

int main()
{
    // 1. Motor de render y ventana principal.
    Render render(960, 540);
    render.initGL();

    // 2. Modo de juego solar: cámara como nave, disparos y colisiones.
    render.solarGameMode = true;
    render.bulletFrs = "data/bullet.frs";
    render.shootCooldown = 0.18f;
    render.shootTimer = 0.0f;

    // 3. Cámara/nave en primera persona.
    Camera* cam = new Camera(
        { 0.0f, 0.0f, 16.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        60.0f * 3.141592f / 180.0f,
        960.0f / 540.0f,
        0.01f,
        100.0f
    );

    cam->moveSpeed = 5.0f;
    render.putCamera(cam);

    // 4. Sol: objeto central, visual y colisionable.
    Sun* sun = new Sun();

    sun->init(
        "data/sun.frs",
        { 0.0f, 0.0f, 0.0f, 1.0f },
        2.2f
    );

    render.putObject(sun);

    // 5. Luz principal situada en el Sol.
    Luz* luz = new Luz(
        { 0.0f, 0.0f, 1.5f, 1.0f },
        { 1.0f, 0.95f, 0.75f, 1.0f },
        0.75f,
        1.00f,
        0.45f
    );

    luz->activa = true;
    render.putLight(luz);

    // 6. Mercurio: planeta pequeño y cercano.
    Planet* mercury = new Planet();

    mercury->init(
        "data/mercury.frs",
        { 3.6f, 0.0f, -1.0f, 1.0f },
        0.35f
    );

    mercury->health = 1;
    mercury->orbit = true;
    mercury->orbitCenter = { 0.0f, 0.0f, -1.0f, 1.0f };
    mercury->orbitRadius = 3.6f;
    mercury->orbitSpeed = 0.65f;
    mercury->orbitAngle = 0.0f;

    render.putObject(mercury);

    // 7. Venus: planeta intermedio con más vida.
    Planet* venus = new Planet();

    venus->init(
        "data/venus.frs",
        { 5.0f, 0.0f, -2.3f, 1.0f },
        0.55f
    );

    venus->health = 2;
    venus->orbit = true;
    venus->orbitCenter = { 0.0f, 0.0f, -2.3f, 1.0f };
    venus->orbitRadius = 5.0f;
    venus->orbitSpeed = 0.45f;
    venus->orbitAngle = 1.2f;

    render.putObject(venus);

    // 8. Tierra: planeta principal, con más vida.
    Planet* earth = new Planet();

    earth->init(
        "data/earth.frs",
        { 6.8f, 0.0f, -3.8f, 1.0f },
        0.60f
    );

    earth->health = 3;
    earth->orbit = true;
    earth->orbitCenter = { 0.0f, 0.0f, -3.8f, 1.0f };
    earth->orbitRadius = 6.8f;
    earth->orbitSpeed = 0.32f;
    earth->orbitAngle = 2.4f;

    render.putObject(earth);

    // 9. Marte: planeta lejano, algo más pequeño.
    Planet* mars = new Planet();

    mars->init(
        "data/mars.frs",
        { 8.6f, 0.0f, -5.5f, 1.0f },
        0.45f
    );

    mars->health = 2;
    mars->orbit = true;
    mars->orbitCenter = { 0.0f, 0.0f, -5.5f, 1.0f };
    mars->orbitRadius = 8.6f;
    mars->orbitSpeed = 0.22f;
    mars->orbitAngle = 3.6f;

    render.putObject(mars);

    // 10. Controles básicos para probar la práctica.
    std::cout << "=====================================\n";
    std::cout << "PR6 EXTRA - Solar Shooter POV\n";
    std::cout << "WASD / Flechas: mover camara-nave\n";
    std::cout << "Raton izquierdo: orientar camara\n";
    std::cout << "F: disparar\n";
    std::cout << "Objetivo: disparar a los planetas\n";
    std::cout << "=====================================\n";

    // 11. Bucle principal del motor.
    render.mainLoop();

    // 12. Limpieza de objetos creados fuera del Render.
    delete cam;
    delete luz;

    cam = nullptr;
    luz = nullptr;

    return 0;
}