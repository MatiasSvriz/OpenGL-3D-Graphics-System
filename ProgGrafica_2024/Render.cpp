#include "Render.h"
#include "Bullet.h"
#include "Planet.h"

#include <iostream>
#include <algorithm>
#include <cstddef>
#include <GLFW/glfw3.h>

Render::Render(int w, int h)
{
    width = w;
    height = h;
}

void Render::initGL()
{
    if (glfwInit() != GLFW_TRUE)
    {
        std::cout << "ERROR: no se pudo iniciar GLFW\n";
        return;
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, "PR5 - Luces y Texturas", nullptr, nullptr);

    if (window == nullptr)
    {
        std::cout << "ERROR: no se pudo crear la ventana GLFW\n";
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    InputManager::init(window);

    glViewport(0, 0, width, height);
    glClearColor(0.08f, 0.08f, 0.10f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Render::putCamera(Camera* camera)
{
    cam = camera;
}

Camera* Render::getCamera()
{
    return cam;
}

void Render::putLight(Luz* light)
{
    luz = light;
}

Luz* Render::getLight()
{
    return luz;
}

void Render::setupObject(Object3D* obj)
{
    if (obj == nullptr)
        return;

    if (obj->vertexList.empty() || obj->idList.empty())
    {
        std::cout << "WARNING: objeto sin vertices o indices\n";
        return;
    }

    BufferObject bo;

    glGenVertexArrays(1, &bo.vao);
    glGenBuffers(1, &bo.vbo);
    glGenBuffers(1, &bo.ebo);

    glBindVertexArray(bo.vao);

    glBindBuffer(GL_ARRAY_BUFFER, bo.vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(Vertex) * obj->vertexList.size(),
        obj->vertexList.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo.ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(unsigned int) * obj->idList.size(),
        obj->idList.data(),
        GL_STATIC_DRAW
    );

    bufferList[obj->objectId] = bo;

    glBindVertexArray(0);
}

void Render::putObject(Object3D* obj)
{
    if (obj == nullptr)
        return;

    setupObject(obj);
    objectList.push_back(obj);
}

void Render::removeObject(Object3D* obj)
{
    if (obj == nullptr)
        return;

    auto itBuffer = bufferList.find(obj->objectId);

    if (itBuffer != bufferList.end())
    {
        BufferObject& bo = itBuffer->second;

        if (bo.ebo != 0)
            glDeleteBuffers(1, &bo.ebo);

        if (bo.vbo != 0)
            glDeleteBuffers(1, &bo.vbo);

        if (bo.vao != 0)
            glDeleteVertexArrays(1, &bo.vao);

        bufferList.erase(itBuffer);
    }

    objectList.erase(
        std::remove(objectList.begin(), objectList.end(), obj),
        objectList.end()
    );
}

void Render::drawGL()
{
    if (cam == nullptr)
        return;

    for (Object3D* obj : objectList)
    {
        if (obj == nullptr)
            continue;

        if (obj->prg == nullptr)
            continue;

        auto itBuffer = bufferList.find(obj->objectId);

        if (itBuffer == bufferList.end())
            continue;

        BufferObject& bo = itBuffer->second;

        obj->prg->use();

        glBindVertexArray(bo.vao);
        glBindBuffer(GL_ARRAY_BUFFER, bo.vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bo.ebo);

        matrix4x4f model = obj->modelMatrix;
        matrix4x4f view = cam->getMatrixLookAt();
        matrix4x4f projection = cam->getMatrixPerspective();
        matrix4x4f mvp = projection * view * model;

        // Se transpone porque nuestra matriz está por filas y OpenGL la interpreta por columnas.
        obj->prg->setMatrix(transpose(mvp), "MVP");
        obj->prg->setMatrix(transpose(model), "M");

        obj->prg->setCamera(cam);
        obj->prg->setLight(luz);
        obj->prg->setMaterial(obj->mat);

        obj->prg->setAttributeData(
            "vPos",
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            (void*)offsetof(Vertex, position)
        );

        obj->prg->setAttributeData(
            "vColor",
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            (void*)offsetof(Vertex, color)
        );

        obj->prg->setAttributeData(
            "vNormal",
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            (void*)offsetof(Vertex, normal)
        );

        obj->prg->setAttributeData(
            "vTextureCoords",
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            (void*)offsetof(Vertex, textureCoords)
        );

        glDrawElements(
            GL_TRIANGLES,
            static_cast<GLsizei>(obj->idList.size()),
            GL_UNSIGNED_INT,
            nullptr
        );

        glBindVertexArray(0);
    }
}

void Render::mainLoop()
{
    if (window == nullptr)
    {
        std::cout << "ERROR: mainLoop llamado sin ventana inicializada\n";
        return;
    }

    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Se actualiza el input una vez por frame.
        InputManager::update();

        if (InputManager::keyState[GLFW_KEY_ESCAPE])
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        if (cam != nullptr)
        {
            vec4float previousPos = cam->pos;
            vec4float previousLookAt = cam->lookAt;

            // La cámara se mueve primero y después se comprueba si la nueva posición es válida.
            cam->move(deltaTime);

            if (solarGameMode)
            {
                bool hitPlanet = !getCollisions(cam->coll, OBJ_PLANET).empty();
                bool hitSun = !getCollisions(cam->coll, OBJ_SUN).empty();

                // Si la cámara choca, se restaura la posición anterior para impedir que atraviese objetos.
                if (hitPlanet || hitSun)
                {
                    cam->pos = previousPos;
                    cam->lookAt = previousLookAt;
                    cam->updateCollider();
                }
            }
        }

        // Se actualizan todos los objetos activos de la escena.
        for (Object3D* obj : objectList)
        {
            if (obj != nullptr && obj->active)
                obj->move(deltaTime);
        }

        if (solarGameMode)
            updateSolarGame(deltaTime);

        // Los objetos desactivados, como balas impactadas o planetas destruidos, se eliminan.
        cleanInactiveObjects();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawGL();

        glfwSwapBuffers(window);
    }

    // Al cerrar la ventana se liberan los buffers creados en GPU.
    for (auto& pair : bufferList)
    {
        BufferObject& bo = pair.second;

        if (bo.ebo != 0)
            glDeleteBuffers(1, &bo.ebo);

        if (bo.vbo != 0)
            glDeleteBuffers(1, &bo.vbo);

        if (bo.vao != 0)
            glDeleteVertexArrays(1, &bo.vao);
    }

    bufferList.clear();

    glfwTerminate();
}

std::vector<Object3D*> Render::getCollisions(Object3D* obj, int objType)
{
    std::vector<Object3D*> collisions;

    if (obj == nullptr || obj->coll == nullptr)
        return collisions;

    // Recorremos la escena buscando objetos activos del tipo pedido.
    for (Object3D* other : objectList)
    {
        if (other == nullptr)
            continue;

        if (other == obj)
            continue;

        if (!other->active)
            continue;

        if (other->tipo != objType)
            continue;

        if (other->coll == nullptr)
            continue;

        // Si ambos colliders se solapan, guardamos el objeto como colisión.
        if (obj->coll->collisionTest(other->coll))
            collisions.push_back(other);
    }

    return collisions;
}

std::vector<Object3D*> Render::getCollisions(Collider* coll, int objType)
{
    std::vector<Object3D*> collisions;

    if (coll == nullptr)
        return collisions;

    // Variante usada cuando queremos comprobar un collider suelto, como el de la cámara.
    for (Object3D* other : objectList)
    {
        if (other == nullptr)
            continue;

        if (!other->active)
            continue;

        if (other->tipo != objType)
            continue;

        if (other->coll == nullptr)
            continue;

        // Se aplica el mismo test de colisión, pero sin depender de un Object3D origen.
        if (coll->collisionTest(other->coll))
            collisions.push_back(other);
    }

    return collisions;
}

void Render::deleteObject(Object3D* obj)
{
    if (obj == nullptr)
        return;

    auto itBuffer = bufferList.find(obj->objectId);

    if (itBuffer != bufferList.end())
    {
        BufferObject& bo = itBuffer->second;

        if (bo.vbo != 0)
            glDeleteBuffers(1, &bo.vbo);

        if (bo.ebo != 0)
            glDeleteBuffers(1, &bo.ebo);

        if (bo.vao != 0)
            glDeleteVertexArrays(1, &bo.vao);

        bufferList.erase(itBuffer);
    }

    objectList.erase(
        std::remove(objectList.begin(), objectList.end(), obj),
        objectList.end()
    );

    delete obj;
}

void Render::cleanInactiveObjects()
{
    for (int i = static_cast<int>(objectList.size()) - 1; i >= 0; i--)
    {
        Object3D* obj = objectList[i];

        if (obj != nullptr && !obj->active)
            deleteObject(obj);
    }
}

Bullet* Render::createBulletFromCamera()
{
    if (cam == nullptr)
        return nullptr;

    // La bala sale en la dirección hacia la que mira la cámara.
    vec4float direction = cam->lookAt - cam->pos;
    direction = normalize(direction);
    direction.w = 0.0f;

    if (direction.x == 0.0f && direction.y == 0.0f && direction.z == 0.0f)
        direction = { 0.0f, 0.0f, -1.0f, 0.0f };

    // La bala nace delante de la cámara para no aparecer dentro de su collider.
    vec4float startPosition = cam->pos + direction * 0.8f;
    startPosition.w = 1.0f;

    Bullet* bullet = new Bullet();

    bullet->init(
        bulletFrs.c_str(),
        startPosition,
        direction,
        8.0f
    );

    if (!bullet->active)
    {
        delete bullet;
        return nullptr;
    }

    // Seguridad: dejamos explícito que este objeto es una bala colisionable.
    bullet->tipo = OBJ_BULLET;
    bullet->active = true;
    bullet->updateModelMatrix();
    bullet->updateCollider();

    return bullet;
}

void Render::updateSolarGame(double deltaTime)
{
    float dt = static_cast<float>(deltaTime);

    // Controla el tiempo mínimo entre disparos.
    if (shootTimer > 0.0f)
        shootTimer -= dt;

    if (shootTimer < 0.0f)
        shootTimer = 0.0f;

    // Si se pulsa F y el cooldown terminó, se crea una bala desde la cámara.
    if (InputManager::keyState[GLFW_KEY_F] && shootTimer <= 0.0f)
    {
        Bullet* bullet = createBulletFromCamera();

        if (bullet != nullptr)
        {
            putObject(bullet);
            shootTimer = shootCooldown;
        }
    }

    size_t objectCount = objectList.size();

    // Recorremos únicamente las balas activas de la escena.
    for (size_t i = 0; i < objectCount; i++)
    {
        Object3D* obj = objectList[i];

        if (obj == nullptr || !obj->active)
            continue;

        if (obj->tipo != OBJ_BULLET)
            continue;

        obj->updateCollider();

        std::vector<Object3D*> planetCollisions = getCollisions(obj, OBJ_PLANET);
        std::vector<Object3D*> sunCollisions = getCollisions(obj, OBJ_SUN);

        if (planetCollisions.empty() && sunCollisions.empty())
            continue;

        // La bala desaparece al impactar contra planeta o Sol.
        obj->active = false;

        // Solo los planetas pierden vida; el Sol solo bloquea la bala.
        for (Object3D* hitObj : planetCollisions)
        {
            Planet* planet = static_cast<Planet*>(hitObj);

            if (planet != nullptr)
                planet->hit();
        }
    }
}