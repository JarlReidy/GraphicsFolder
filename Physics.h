#pragma once
#include "stdafx.h"
#include "TransformNode.h"
#include <vector>
#include "CollisionGrid.h"
#include "Terrain.h"
//#define USE_BROADPHASE
//using namespace std;

struct ContactInfo
{
    glm::vec3 normal;
    float depth;
};

class StaticCollider
{
    
    // abstract interface for something that static that collides with a sphere
public:
    //virtual bool Collide(const glm::vec3& r, float rad, glm::vec3 &normal, float &depth) const = 0;
    virtual bool Collide(const glm::vec3& r, float rad, glm::vec3 &normal, float &depth) const = 0;
    virtual ~StaticCollider() {}
    //virtual void GetAABB(glm::vec3& vMin, glm::vec3& vMax) const = 0;

    
    
};
/*
class TerrainCollider : public StaticCollider
{
    MemTexture* memTexture;
    glm::vec3 scales;
    SOF::Texture* heightMap;

    

public:

    int width = heightMap->GetWidth();
    int height = heightMap->GetHeight();

    std::vector<glm::vec2> uv;
    TerrainCollider(const std::string& heightMapFile, const glm::vec3 scales) : scales(scales)
    {
        // load texture here
        heightMap = new SOF::Texture(heightMapFile);
        
    }
    ~TerrainCollider() 
    { 
        delete memTexture; 
    }

    bool Collide(const glm::vec3& p, float rad, glm::vec3& normal, float& depth) const override
    {
        // use the heightmap to detect collision with a sphere

    }

};*/

class QuadCollider : public StaticCollider
{
    glm::vec3 r, a, b;
    glm::vec3 aHat, bHat; // unit vectors along sides a and b
    float lena, lenb; // lengths of sides a and b
    //glm::vec3 corners[4] = { r,r + a,r + b,r + a + b };
public:
    QuadCollider(const glm::vec3& r, const glm::vec3& a, const glm::vec3& b) : r(r), a(a), b(b)
    {
        lena = glm::length(a);
        lenb = glm::length(b);
        aHat = a / lena;
        bHat = b / lenb;
    }
    QuadCollider(glm::mat4 trans)
    {
        // this constructor assumes a unit quad centered on the origin with sides x, z
        // transform transforms this into place
        glm::vec3 rt = trans * glm::vec4(-0.5f, 0.0f, -0.5f, 1.0f);
        glm::vec3 at = trans * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 bt = trans * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
        r = glm::vec3(rt.x, rt.y, rt.z);
        a = glm::vec3(at.x, at.y, at.z);
        b = glm::vec3(bt.x, bt.y, bt.z);
        lena = glm::length(a);
        lenb = glm::length(b);
        aHat = a / lena;
        bHat = b / lenb;
    }
    ~QuadCollider() {}

    bool Collide(const glm::vec3& p, float rad, glm::vec3& normal, float& depth) const override
    {
        float la, lb;
        la = dot(p - r, aHat);
        lb = dot(p - r, bHat);
        if (la < 0.0f) la = 0.0f; if (la > lena) la = lena;
        if (lb < 0.0f) lb = 0.0f; if (lb > lenb) lb = lenb;
        
        glm::vec3 rNear = r + aHat * la + bHat * lb;

        if (glm::dot(p - rNear, p - rNear) < rad * rad)
        {
            normal = glm::normalize(p - rNear);
            depth = rad - glm::length(p - rNear);
            return true;
        }
        else
        {
            return false;
        }
    }
};

struct Projectile
{
    glm::vec3 pos;
    glm::vec3 vel;
    float radius;
    float restitution;
    TransformNode* sceneGraphNode;
    Projectile(const glm::vec3& pos, const glm::vec3& vel,float radius, float restitution, TransformNode* sgNode) : pos(pos), vel(vel), radius(radius), restitution(restitution),sceneGraphNode(sgNode) {}
    ~Projectile()
    {
        // disconnect and delete the scene graph node
        delete sceneGraphNode;
    }
};

// scene graph update callback for physics objects
class PhysicsCallback : public UpdateCallback
{
    TransformNode* sgNode;
    Projectile* physics;
    
public:
    PhysicsCallback(TransformNode* sgNode, Projectile* physics) : sgNode(sgNode), physics(physics) {}
    virtual void operator()(float dt)
    {
        // copy the position from one to the other
        sgNode->SetTransform(glm::translate(glm::mat4(), physics->pos));
    }
};

/*
class SphereCollider : public StaticCollider
{
    glm::vec3 r;
    float rad;

public:
    SphereCollider(const glm::vec3& r, float rad) : r(r), rad(rad) {}
    bool Collide(const glm::vec3& p, float rad2, glm::vec3& normal, float& depth) const override
    {
        if (glm::dot(p - r, p - r) <= (rad + rad2) * (rad + rad2))
        {
            float l = glm::length(p - r);
            depth = rad + rad2 - l;
            normal = (p - r) / l;
            return true;
        }
        return false;
    }
    ~SphereCollider(){}

    glm::vec3 xMin = r - rad;
    glm::vec3 xMax = r + rad;


    void GetAABB(const glm::vec3& xMin, const glm::vec3& xMax) const 
    {
        
    }

};
*/

    class Physics
    {
        std::vector<StaticCollider*> staticColliders;//a vector of colliders which 
        //we can add.
        std::vector<Projectile*> projectiles;// a vector of projectiles we can shoot

        glm::vec3 gravity = glm::vec3(0.0f, -10.0f, 0.0f);

        int collisionTests;//the number of collisions a projectile records

        CollisionGrid grid = CollisionGrid(-50.0f, -50.0f, -50.0f, 50.0f, 2.0f);
    public:
        void AddCollider(StaticCollider* coll) 
        { 
 /*
#ifdef USE_BROADPHASE
            glm::vec3 vMin, vMax;
            coll->GetAABB(vMin, vMax);
            grid.AddCollider(coll, vMin.x, vMax.x, vMin.z, vMax.z);
#else
            staticColliders.push_back(coll);
#endif*/
            staticColliders.push_back(coll);
        }

        std::vector<ContactInfo>CollideWithWorld(const glm::vec3& spherePos, float sphereRad);

        void AddProjectile(Projectile* proj)
        {
            projectiles.push_back(proj);
            proj->sceneGraphNode->SetUpdateCallback(new PhysicsCallback(proj->sceneGraphNode, proj));
        }

        void Update(float deltaTime);

        int collisionCounter() const { return collisionTests; }
        //return an interger value incremented by collisions
    };
