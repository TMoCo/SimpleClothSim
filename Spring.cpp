// class declaration of the spring
#include "Spring.h"

#include <iostream>

// glm maths
//#include <glm/geometric.hpp>

// create a spring between two balls
Spring::Spring(PointMass* left, PointMass* right, float stiffness, float damper)
{
    // update pointers
    left_ = left;
    right_ = right;
    // initialise current and rest length
    rest_ = glm::distance(right->position_, left->position_);
    curr_ = rest_;
    // arbitrary constants
    k_ = stiffness;
    d_ = damper;
}

// destructor
Spring::~Spring()
{
    // do something
}

// compute the force exerced by the spring in Newtons
void Spring::UpdateParticles()
{
    // compute current length
    curr_ = glm::distance(right_->position_, left_->position_);
    // unit vector for the spring force direction (from left to right)
    glm::vec3 spring = glm::normalize(right_->position_ - left_->position_); 
 
    // compute the forces applied on both ends of the spring
    glm::vec3 force = 
    (-k_ * (curr_ - rest_) // spring force
    - d_ * glm::dot(right_->velocity_ - left_->velocity_, spring)) // dampening (project relative velocity onto the spring)
    * spring;
    
    // apply the forces to the corresponding masses along with air resistance
    left_->net_F_ -= force;
    right_->net_F_ += force;
}

void Spring::UpdateParticles(float k, float d)
{
    k_ = k;
    d_ = d;
    UpdateParticles();
}

std::ostream & operator << (std::ostream &outStream, const Spring &spring)
{
    outStream << "spring links " << *spring.left_ << " to " << *spring.right_;
    return outStream; 
}