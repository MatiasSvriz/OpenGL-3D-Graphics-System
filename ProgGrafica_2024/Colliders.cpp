#include "Colliders.h"

bool Sphere::collisionTest(Collider* coll)
{
    Sphere* other = dynamic_cast<Sphere*>(coll);

    if (other == nullptr)
        return false;

    float dx = center.x - other->center.x;
    float dy = center.y - other->center.y;
    float dz = center.z - other->center.z;

    float distanceSquared = dx * dx + dy * dy + dz * dz;
    float radiusSum = radius + other->radius;

    return distanceSquared <= radiusSum * radiusSum;
}

void Sphere::addParticle(particle3D* p)
{
    if (p == nullptr)
        return;

    particleList.push_back(p);

    float pMaxX = p->pos.x + p->size.x / 2.0f;
    float pMaxY = p->pos.y + p->size.y / 2.0f;
    float pMaxZ = p->pos.z + p->size.z / 2.0f;

    float pMinX = p->pos.x - p->size.x / 2.0f;
    float pMinY = p->pos.y - p->size.y / 2.0f;
    float pMinZ = p->pos.z - p->size.z / 2.0f;

    if (pMaxX > max.x) max.x = pMaxX;
    if (pMaxY > max.y) max.y = pMaxY;
    if (pMaxZ > max.z) max.z = pMaxZ;

    if (pMinX < min.x) min.x = pMinX;
    if (pMinY < min.y) min.y = pMinY;
    if (pMinZ < min.z) min.z = pMinZ;

    vec4float size = max - min;

    radius = length(size) / 2.0f;

    centerPrime = min + size / 2.0f;
    centerPrime.w = 1.0f;

    center = centerPrime;
}

void Sphere::updateCollider(matrix4x4f mat)
{
    center = mat * centerPrime;

    vec4float worldMax = mat * max;
    vec4float worldMin = mat * min;

    radius = length(worldMax - worldMin) / 2.0f;

    // Evita que objetos pequeños, como balas, tengan un collider casi nulo.
    if (radius < 0.15f)
        radius = 0.15f;
}