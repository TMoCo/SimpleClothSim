#ifndef SPRING_H
#define SPRING_H

// a point mass particle class
#include "PointMass.h"

// spring class connecting to point masses
class Spring
{
    public:
    // constructor
    Spring(PointMass* right, PointMass* left, float stiffness, float damper);
    // destructor
    ~Spring();

    // compute the force exerced by the spring in Newtons
    void UpdateParticles();
    // overload for new k and d
    void UpdateParticles(float k, float d);

    // spring scalars (stiffness, damper, viscosity)
    float k_;
    float d_;

    // spring lengths
    float rest_;
    float curr_;

    // pointers to the balls the spring is connected to
    PointMass* right_;
    PointMass* left_;
};

std::ostream & operator << (std::ostream &outStream, const Spring &spring);


#endif