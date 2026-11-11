#include "Camera.h"
#include "InputManager.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

Camera::Camera(vec4float pos, vec4float rot,
    vec4float lookAt, vec4float up,
    float fovy, float aspectRatio,
    float zNear, float zFar)
    : pos(pos), rot(rot), lookAt(lookAt), up(up),
    fovy(fovy), aspectRatio(aspectRatio), zNear(zNear), zFar(zFar),
    coll(nullptr)
{
    // También creamos su colisionador.
    createCollider();

    std::cout << "[DEBUG CAMERA] Collider de camara creado\n";
}

// Matriz Vista
matrix4x4f Camera::getMatrixLookAt()
{
    vec4float f = normalize(lookAt - pos);          // forward
    vec4float r = normalize(cross(f, up));           // right
    vec4float u = cross(r, f);                       // up real

    matrix4x4f m = make_identityf();

    // Fila 0: right
    set(m, 0, 0, r.x); set(m, 0, 1, r.y); set(m, 0, 2, r.z);
    set(m, 0, 3, -dot(r, pos));

    // Fila 1: up
    set(m, 1, 0, u.x); set(m, 1, 1, u.y); set(m, 1, 2, u.z);
    set(m, 1, 3, -dot(u, pos));

    // Fila 2: -forward (OpenGL mira hacia -Z)
    set(m, 2, 0, -f.x); set(m, 2, 1, -f.y); set(m, 2, 2, -f.z);
    set(m, 2, 3, dot(f, pos));

    // Fila 3: ya es [0,0,0,1] por make_identityf

    return m;
}

// Matriz de proyeccion perspectiva
matrix4x4f Camera::getMatrixPerspective()
{
    float tanHalfFovy = tanf(fovy * 0.5f);

    matrix4x4f m{};  // todo ceros

    set(m, 0, 0, 1.0f / (aspectRatio * tanHalfFovy));
    set(m, 1, 1, 1.0f / tanHalfFovy);
    set(m, 2, 2, -(zFar + zNear) / (zFar - zNear));
    set(m, 2, 3, -(2.0f * zFar * zNear) / (zFar - zNear));
    set(m, 3, 2, -1.0f);

    return m;
}

// Movimiento de camara FPS:
//   - Flechas: mover posicion en el plano de vista
//   - Raton: rotar el punto al que mira
void Camera::move(double timeStep)
{
    // Convertimos el delta de tiempo a float para trabajar cómodamente
    // con el resto de magnitudes de movimiento/rotación.
    float dt = (float)timeStep;

    // 1) Calcular la dirección actual de la cámara
    // lookAt - pos nos da el vector que va desde la posición de la
    // cámara hasta el punto al que mira.
    // Lo normalizamos para quedarnos solo con la dirección.
    vec4float f = normalize(lookAt - pos);

    // 2) Rotación con el ratón
    static bool firstMouse = true;

    // Guardan la última posición conocida del ratón para poder calcular el desplazamiento entre frames.
    static double lastMX = 0.0;
    static double lastMY = 0.0;

    // Solo rotamos la cámara si está pulsado el botón izquierdo.
    if (InputManager::mouseState[GLFW_MOUSE_BUTTON_LEFT])
    {
        if (firstMouse)
        {
            // Guardamos la posición anterior del ratón.
            lastMX = InputManager::posMX;
            lastMY = InputManager::posMY;
            firstMouse = false;
        }

        // dx = cuánto se ha movido el ratón horizontalmente
        // dy = cuánto se ha movido verticalmente
        // Multiplicamos por la sensibilidad para controlar cuánto gira la cámara.
        float dx = (float)(InputManager::posMX - lastMX) * mouseSensX;
        float dy = (float)(InputManager::posMY - lastMY) * mouseSensY;

        // Actualizamos la última posición conocida del ratón para el siguiente frame.
        lastMX = InputManager::posMX;
        lastMY = InputManager::posMY;

        // Rotación respecto a ejes globales
		// dy rota en X (pitch), dx rota en Y (yaw)
        matrix4x4f rot = make_rotateY(dx) * make_rotateX(dy);

		// Giramos el vector de dirección de la cámara con la matriz de rotación.
        f = rot * f;
        // Normalizamos por seguridad, la matriz de rotación deberia mantener la longitud.
        f = normalize(f);
        f.w = 0.0f;
    }
    else
    {
        firstMouse = true;
    }

    // 3. Vector derecha de la cámara
    vec4float r = normalize(cross(f, up));
    r.w = 0.0f;

    // 4. Movimiento con teclado
    float v = moveSpeed * dt;

    if (InputManager::keyState[GLFW_KEY_W] || InputManager::keyState[GLFW_KEY_UP])
        pos = pos + f * v;

    if (InputManager::keyState[GLFW_KEY_S] || InputManager::keyState[GLFW_KEY_DOWN])
        pos = pos - f * v;

    if (InputManager::keyState[GLFW_KEY_D] || InputManager::keyState[GLFW_KEY_RIGHT])
        pos = pos + r * v;

    if (InputManager::keyState[GLFW_KEY_A] || InputManager::keyState[GLFW_KEY_LEFT])
        pos = pos - r * v;

    // 5. Actualizar lookAt
    lookAt = pos + f;
    lookAt.w = 1.0f;

    updateCollider();

}
void Camera::createCollider()
{
    if (coll != nullptr)
    {
        for (size_t i = 0; i < coll->particleList.size(); i++)
        {
            delete coll->particleList[i];
            coll->particleList[i] = nullptr;
        }

        coll->particleList.clear();

        delete coll;
        coll = nullptr;
    }

    Sphere* sphere = new Sphere();

    particle3D* particle = new particle3D();

    // La cámara se aproxima con una pequeña esfera envolvente.
    particle->pos = {
        0.0f,
        0.0f,
        0.0f,
        1.0f
    };

    // Tamaño para representar el volumen físico de la cámara/nave.
    particle->size = {
        0.25f,
        0.25f,
        0.25f,
        0.0f
    };

    sphere->addParticle(particle);

    coll = sphere;

    updateCollider();
}

void Camera::updateCollider()
{
    if (coll == nullptr)
        return;

    // El collider sigue la posición actual de la cámara.
    matrix4x4f T = make_translate(pos.x, pos.y, pos.z);

    coll->updateCollider(T);
}