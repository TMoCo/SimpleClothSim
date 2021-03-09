// Floor.cpp
#include "Collidable.h"

// opengGL functions
#include <GL/gl.h>
#include <GL/glu.h>

//
// Collidable Base Class
//

Collidable::Collidable(float friction_s, float friction_k, float size, glm::vec3 position)
{
    static_friction_ = friction_s;
    kinetic_friction_ = friction_k;
    size_ = size;
    position_ = position;
}

Collidable::~Collidable()
{

}

//
// Floor Class
//

Floor::Floor(float friction_s, float friction_k, float size, glm::vec3 position)
    : Collidable(friction_s, friction_k, size, position)
{

}

Floor::~Floor()
{
    
}

// checks whether a point mass has collided with the floor
void Floor::ComputeCollision(PointMass* point, float gravity)
{
    // simple case is the floor on the xz plane
    if (point->position_.y <= position_.y + 0.1)
    {
        // place point on the floor
        point->position_.y = position_.y + 0.1;
        // get the normal force
        float normal_force = point->mass_ * gravity;
        // which give us the maximum static friction
        float max_friction = static_friction_ * normal_force;
        // force pointing downwards (into the floor) so project on the floor
        if (point->net_F_.y < 0)
        {
            // unit force direction in xz plane
            glm::vec3 force_dir = glm::normalize(glm::vec3(point->net_F_.x, 0, point->net_F_.z));
            // project force onto floor plane along force's xz vector 
            glm::vec3 projected_force = glm::dot(point->net_F_, force_dir) * force_dir;
            float delta = max_friction - projected_force.length();
            // if force is greater than max friction
            if (delta < 0)
            {
                float friction = kinetic_friction_ * normal_force;
                // friction is in the opposite direction of the force
                point->net_F_ = projected_force - friction * force_dir;
            }
            else 
            {
                // force is smaller than friction so friction (oppose projected force) wins
                point->velocity_ = glm::vec3(0);
                point->net_F_ = glm::vec3(0);
            }
        }
    }
}

void Floor::DrawCollidable()
{
    // draw two triangles
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0, 1.0, 0.0);
    // triangle 1
    glVertex3f(-size_, 0, size_);
    glVertex3f(size_, 0, -size_);
    glVertex3f(size_, 0, size_);
    // triangle 2
    glVertex3f(-size_, 0, -size_);
    glVertex3f(size_, 0, -size_);
    glVertex3f(-size_, 0, size_);
    glEnd();
}

//
// Sphere Class
//

Sphere::Sphere(float friction_s, float friction_k, float size, glm::vec3 position)
    : Collidable(friction_s, friction_k, size, position)
{

}

Sphere::~Sphere()
{
    
}

// checks whether a point mass has collided with sphere
void Sphere::ComputeCollision(PointMass* point, float gravity)
{
    // check if the distance from the point to the centre of the sphere is smaller than radius
    if (glm::distance(point->position_, position_) < size_)
    {
        // place point on the sphere surface
        glm::vec3 to_surface = glm::normalize(point->position_ - position_); 
        point->position_ = position_ + to_surface * size_;
        // for now, settle with having positions fixed on contact
        point->net_F_ = glm::vec3(0);
        point->velocity_= glm::vec3(0);
        // get the unit tangent to the sphere from cross product of normal with a xz vector
        //glm::vec3 tangent = glm::cross(to_surface, glm::vec3(0.5, 0, 0.5));
        // make sure tangent vector is pointing down
        //if (glm::dot(tangent, glm::vec3(0, -1, 0)) < 0)
        //    tangent = -tangent;
        // project forces onto the tangent

    }

}

void Sphere::DrawCollidable()
{
    // set up the quadric object for the sphere
    static GLUquadricObj* quad_obj = NULL;
	if (quad_obj == NULL)
		quad_obj = gluNewQuadric();
	gluQuadricDrawStyle(quad_obj, GLU_FILL);
	gluQuadricNormals(quad_obj, GLU_SMOOTH);

    // draw a low resolution sphere
    glPushMatrix();
    glTranslatef(position_.x, position_.y, position_.z);
    gluSphere(quad_obj, size_, 10, 10);
    glPopMatrix();
}




