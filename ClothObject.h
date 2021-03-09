#ifndef CLOTH_OBJECT_H
#define CLOTH_OBJECT_H

// include the C++ standard libraries we need for the header
#include <vector>
#include <iostream>

// openGL
#include <GL/gl.h>

// glm maths
#include <glm/glm.hpp>

// classes for modelling a cloth 
#include "PointMass.h"
#include "Spring.h"

class ClothObject
{
    // POD face struct, a triangle
    struct Triangle
    {
        // triangle index
        unsigned int index;
        // arrays containing the face vertex indices
        unsigned int positions[3];
        unsigned int normals[3];
        unsigned int textures[3];
    };

    // just three integer values for RGB
    struct RGB
    {
        unsigned char red;
        unsigned char green;
        unsigned char blue;
    };

    // bit flags for object file properties
    enum Properties : unsigned int
    {
        kHasTextures = 1,
        kHasNormals = 2,
    };


    public:
    // constructor
    ClothObject();
    // destructor
    ~ClothObject();
    

    // read routine returns true on success, failure otherwise
    bool ReadObject(std::string &obj_file);
    bool ReadTexture(std::string &ppm_file);
    // write routine
    void WriteObject(std::string &obj_file);
    void ClearObject();
    
    // methods for openGL
    void SetTexture();
    void Render();
    void ShowPoints();
    void ComputeNormals();

    // checks whether a mass a is linked to another mass b
    bool CheckPointSprings(PointMass* point_a, unsigned int index_b);
    // generate data for a rectangular piece of cloth
    void GenClothGrid(int height, int width, float size);
    void ComputeForces(glm::vec3 gravity, glm::vec3 wind, float air_res);


    // vertex vectors
    std::vector<glm::vec3> vertices_;
    std::vector<glm::vec3> normals_;
    std::vector<glm::vec3> texture_coords_;

    // cloth vectors
    std::vector<PointMass*> mass_particles_;
    std::vector<Spring*> springs_;

    // face vector 
    std::vector<Triangle*> triangles_;

    // RGB texture
    RGB* texture_;
    // texture dimensions
    int width_, height_;
    // a variable to store the texture's ID on the GPU
    GLuint texture_id_;

    // computed object characteristics
    glm::vec3 centre_of_gravity_;
    float object_size_;
    float target_size_;

    // object properties
    float cloth_mass_;
    float cloth_k_;
    float cloth_d_;
    float cloth_gravity_; 
    float cloth_air_;
    float cloth_wind_;
    float y_pos_;

    // bit mask containing object properties
    unsigned int object_properties_;
};

#endif  // CLOTH_OBJECT_H