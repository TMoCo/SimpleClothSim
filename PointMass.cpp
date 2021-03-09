// class definition
#include "PointMass.h"

// glut sphere 
#include <GL/freeglut.h>

//
#include <iostream>

// constructor sets the initial conditions of the particle
PointMass::PointMass(float mass, glm::vec3 velocity, glm::vec3 position)
{
    mass_ = mass;
    velocity_ = velocity;
    position_ = position;
    net_F_ = glm::vec3(0);
    index = 0;
}

PointMass::~PointMass()
{
    // do something
}

void PointMass::DrawPoint()
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
    gluSphere(quad_obj, 0.1, 10, 10);
    glPopMatrix();
}

std::ostream & operator << (std::ostream &outStream, const PointMass &point_mass)
{
    outStream << "mass " << point_mass.index;
    return outStream;
}
// add extra methods?