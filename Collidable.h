// Floor.h
#ifndef COLLIDABLE_H
#define COLLIDABLE_H

#include "PointMass.h"

class Collidable
{
    public: 
    // constructor
    Collidable(float friction_s, float friction_k, float size, glm::vec3 position);
    // destructor
    ~Collidable();

    // pure virtual functions
    virtual void ComputeCollision(PointMass *point, float gravity) =0;
    virtual void DrawCollidable() =0;

    // collidable in worls space
    glm::vec3 position_;
    // dimension of the collidable
    float size_;
    // friction constants
    float static_friction_;
    float kinetic_friction_;

};

// class for computing floor collision
class Floor : public Collidable
{
    public:

    // constructor
    Floor(float friction_s, float friction_k, float size, glm::vec3 position);
    // destructor
    ~Floor();

    // overload methods for collision and render
    void ComputeCollision(PointMass *point, float gravity);
    void DrawCollidable();
};

// class for computing floor collision
class Sphere : public Collidable
{
    public:

    // constructor
    Sphere(float friction_s, float friction_k, float size, glm::vec3 position);
    // destructor
    ~Sphere();

    // overload methods for collision and render
    void ComputeCollision(PointMass *point, float gravity);
    void DrawCollidable();
};

#endif