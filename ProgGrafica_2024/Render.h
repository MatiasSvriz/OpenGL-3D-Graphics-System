#pragma once
#include "common.h"
#include "Object3D.h"
#include "InputManager.h"
#include "Camera.h"
#include "Luz.h"
#include "Bullet.h"

#include <string>
#include <vector>
#include <map>

class Render
{
public:

    // Buffer GPU asociado a un Object3D
    struct BufferObject
    {
        unsigned int vao = 0;
        unsigned int vbo = 0;
        unsigned int ebo = 0;
    };

    // Ventana GLFW
    GLFWwindow* window = nullptr;

    // Objetos de la escena
    std::vector<Object3D*> objectList;

    // Mapa objectId -> buffers GPU
    std::map<unsigned int, BufferObject> bufferList;

    // Camara y luz de la escena
    Camera* cam = nullptr;
    Luz* luz = nullptr;

    // Resolucion de ventana
    int width = 640;
    int height = 480;

    // Activa el modo de juego solar.
    // En este modo, la cámara representa la nave.
    bool solarGameMode = false;

    // Fichero FRS usado para crear las balas.
    std::string bulletFrs = "bullet.frs";

    // Control de cadencia de disparo.
    float shootCooldown = 0.25f;
    float shootTimer = 0.0f;

    // Crea una bala desde la posición y dirección de la cámara.
    Bullet* createBulletFromCamera();

    // Actualiza la lógica del minijuego.
    void updateSolarGame(double deltaTime);

    // Constructor
    Render(int w, int h);

    // Inicializacion
    void initGL();

    // Camara / luz
    void putCamera(Camera* camera);
    Camera* getCamera();

    void putLight(Luz* light);
    Luz* getLight();

    // Objetos
    void setupObject(Object3D* obj);
    void putObject(Object3D* obj);
    void removeObject(Object3D* obj);

    // Renderizado
    void drawGL();
    void mainLoop();

    std::vector<Object3D*> getCollisions(Object3D* obj, int objType);

    std::vector<Object3D*> getCollisions(Collider* coll, int objType);

    void deleteObject(Object3D* obj);

    void cleanInactiveObjects();

};