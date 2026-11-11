#pragma once

#include "common.h"
#include "libMath.h"

#include <vector>

using namespace std;

typedef struct
{
    vec4float pos;
    vec4float size;
} particle3D;

class Collider
{
public:
    vector<particle3D*> particleList;

    vec4float max = { -1000000.0f, -1000000.0f, -1000000.0f, 0.0f };
    vec4float min = { 1000000.0f, 1000000.0f, 1000000.0f, 0.0f };

    Collider() {}

    virtual ~Collider() {}

    virtual bool collisionTest(Collider* coll) = 0;
    virtual void addParticle(particle3D* particle) = 0;
    virtual void updateCollider(matrix4x4f mat) = 0;
};

class Sphere : public Collider
{
public:
    float radius = 0.0f;

    vec4float center = { 0.0f, 0.0f, 0.0f, 1.0f };
    vec4float centerPrime = { 0.0f, 0.0f, 0.0f, 1.0f };

    Sphere() {}

    virtual bool collisionTest(Collider* coll) override;
    virtual void addParticle(particle3D* particle) override;
    virtual void updateCollider(matrix4x4f mat) override;
};

typedef struct colliderNode
{
    Collider* bv = nullptr;
    colliderNode* sons[2] = { nullptr, nullptr };
} colliderNode;

template <class TypeCollider>
class ColliderTree
{
public:
    colliderNode* root = nullptr;

    ColliderTree(colliderNode* root)
    {
        this->root = root;
    }

    static bool collisionTest(colliderNode* coll, colliderNode* otherColl)
    {
        if (coll == nullptr || otherColl == nullptr)
            return false;

        if (coll->bv == nullptr || otherColl->bv == nullptr)
            return false;

        if (!coll->bv->collisionTest(otherColl->bv))
            return false;

        bool collHasSons = coll->sons[0] != nullptr || coll->sons[1] != nullptr;
        bool otherHasSons = otherColl->sons[0] != nullptr || otherColl->sons[1] != nullptr;

        if (!collHasSons && !otherHasSons)
            return true;

        bool collisionDetected = false;

        if (collHasSons && otherHasSons)
        {
            for (int i = 0; i < 2; i++)
            {
                if (coll->sons[i] == nullptr)
                    continue;

                for (int j = 0; j < 2; j++)
                {
                    if (otherColl->sons[j] == nullptr)
                        continue;

                    collisionDetected |= collisionTest(coll->sons[i], otherColl->sons[j]);
                }
            }
        }
        else if (collHasSons)
        {
            for (int i = 0; i < 2; i++)
            {
                if (coll->sons[i] != nullptr)
                    collisionDetected |= collisionTest(coll->sons[i], otherColl);
            }
        }
        else
        {
            for (int j = 0; j < 2; j++)
            {
                if (otherColl->sons[j] != nullptr)
                    collisionDetected |= collisionTest(coll, otherColl->sons[j]);
            }
        }

        return collisionDetected;
    }

    static void subdivide(colliderNode* node)
    {
        if (node == nullptr || node->bv == nullptr)
            return;

        if (node->bv->particleList.size() <= 1)
        {
            node->sons[0] = nullptr;
            node->sons[1] = nullptr;
            return;
        }

        colliderNode* n1 = new colliderNode;
        colliderNode* n2 = new colliderNode;

        n1->bv = new TypeCollider();
        n2->bv = new TypeCollider();

        vec4float center = ((TypeCollider*)node->bv)->center;
        vec4float nodeSize = node->bv->max - node->bv->min;

        int axis = 0;

        if (nodeSize.y >= nodeSize.x && nodeSize.y >= nodeSize.z)
            axis = 1;

        else if (nodeSize.z >= nodeSize.x && nodeSize.z >= nodeSize.y)
            axis = 2;

        for (particle3D* p : node->bv->particleList)
        {
            if (axis == 0 && p->pos.x < center.x)
                n1->bv->addParticle(p);

            else if (axis == 1 && p->pos.y < center.y)
                n1->bv->addParticle(p);

            else if (axis == 2 && p->pos.z < center.z)
                n1->bv->addParticle(p);

            else
                n2->bv->addParticle(p);
        }

        if (!n1->bv->particleList.empty() && !n2->bv->particleList.empty())
        {
            node->sons[0] = n1;
            node->sons[1] = n2;

            subdivide(n1);
            subdivide(n2);

            return;
        }

        delete n1->bv;
        delete n2->bv;

        delete n1;
        delete n2;

        node->sons[0] = nullptr;
        node->sons[1] = nullptr;
    }

    static void updateTree(colliderNode* root, matrix4x4f mat)
    {
        if (root == nullptr || root->bv == nullptr)
            return;

        root->bv->updateCollider(mat);

        if (root->sons[0] != nullptr)
            updateTree(root->sons[0], mat);

        if (root->sons[1] != nullptr)
            updateTree(root->sons[1], mat);
    }
};