#ifndef POINT_MASS_H
#define POINT_MASS_H

// glm vec3 vectors 
#include <glm/glm.hpp>

// container for the springs linked to this particle
#include <vector>

#include <iostream>

// POD Class for a mass particle used in the cloth simulation
class PointMass
{
    public:

    // constructor for a mass particle
    PointMass(float mass, glm::vec3 velocity, glm::vec3 position);
    // destructor
    ~PointMass();

    // draw a sphere at the point's location
    void DrawPoint();

    // particle index for comparison
    unsigned int index;

    // the particle's mass
    float mass_;

    // sum of all the forces applied to the particle
    glm::vec3 net_F_;

    // particle's velocity
    glm::vec3 velocity_;

    // a pointer to a vertex position
    glm::vec3 position_;

    // a vector containing the ids of the the springs this particle is connected to 
    std::vector<unsigned int> spring_indices_;
};

std::ostream & operator << (std::ostream &outStream, const PointMass &p_mass);

#endif