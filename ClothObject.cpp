// include the header file
#include "ClothObject.h"

// include the C++ standard libraries we want
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#define MAXIMUM_LINE_LENGTH 1024

// constructor
ClothObject::ClothObject() : centre_of_gravity_(0.0,0.0,0.0)
{
    // geometry data
    vertices_.resize(0);
    normals_.resize(0);
    texture_coords_.resize(0);
    triangles_.resize(0);
    object_size_ = 1.0;
    target_size_ = 2.0;

    // texture data
    texture_ = NULL;
    width_ = height_ = 0;

    // cloth data
    mass_particles_.resize(0);
    springs_.resize(0);

    // cloth properties
    cloth_mass_ = 1;
    cloth_k_ = 10;
    cloth_d_ = 0.5;
    cloth_gravity_ = 9.8;
    cloth_air_ = 0;
    cloth_wind_ = 0;
    y_pos_ = 1.5;

    // model properties bit mask
    object_properties_ = 0;
}

// destructor
ClothObject::~ClothObject()
{
    delete texture_;
}

//
// File I/O (.obj and .ppm (P3))
//

void ClothObject::ClearObject()
{
    // resize our data
    vertices_.resize(0);
    normals_.resize(0);
    texture_coords_.resize(0);
    triangles_.resize(0);
    mass_particles_.resize(0);
    springs_.resize(0);
    centre_of_gravity_ = glm::vec3(0);
}

bool ClothObject::ReadObject(std::string &obj_file)
{   
    // create a read buffer
    char read_buffer[MAXIMUM_LINE_LENGTH];
    // utility char* for tokenising
    char* token;
    unsigned int current_triangle = 0;

    // open a file stream
    std::ifstream file;
    file.open(obj_file, std::ios::in);

    // return false if we couldn't open the obj file
    if (!file.is_open())
        return false;

    // resize our data
    vertices_.resize(0);
    normals_.resize(0);
    texture_coords_.resize(0);
    triangles_.resize(0);
    mass_particles_.resize(0);
    springs_.resize(0);
    object_properties_ = 0;

    // loop one line at a time until EOF
    while (!file.eof())
    {
        // read in the line
        file.getline(read_buffer, MAXIMUM_LINE_LENGTH);
        
        // check the first element of the line
        switch (read_buffer[0])
        {
            // vertex data
            case 'v':
            {
                // get the first chars of the line, identifies entry type (here we are only considering v, vn, vt, f)
                // this also initialise tokenising on read buffer so subsequent calls will tokenise it
                char* type = strtok(read_buffer, " ");
                // create a vertex and read in the next values
                glm::vec3 vec;
                // tokenise the values as floats (x,y,z)
                token = strtok(NULL, " ");
                vec.x = token ? atof(token) : 0;
                token = strtok(NULL, " ");
                vec.y = token ? atof(token) : 0;
                token = strtok(NULL, " ");
                vec.z = token ? atof(token) : 0;
                // now use the second character to choose branch (plain vertex, normal or texture)
                if (strlen(type) == 1)
                    vertices_.push_back(vec);
                else if (type[1] == 'n')
                {
                    if (!(object_properties_ & kHasNormals))
                        object_properties_ += kHasNormals;
                    normals_.push_back(vec);
                }
                else if (type[1] == 't')
                {
                    if (!(object_properties_ & kHasTextures))
                        object_properties_ += kHasTextures;
                    texture_coords_.push_back(vec);
                }
                break;
            }
            case 'f':
            {
                // get the line as a string for further porcessing
                std::string line_string(read_buffer);
                
                // find out how many vertices there are in the face
                int vertices = 0;
                char buffer[line_string.length()];

                strcpy(buffer, read_buffer);
                token = strtok(buffer, " ");
                while ((token = strtok(NULL, " ")) != NULL)
                    vertices++;

                // skip line if we don't have enough vertices to process
                if (vertices < 3)
                    continue;

                // vertex extraction variables
                size_t v0_pos, v1_pos, v2_pos; // data position in line
                std::string v0, v1, v2; // vertex data as string
                int triangle_data[9]; // triangle data as int

                // the position of the first vertex, which we store as we will use it to fan out when there are more 
                // than three vertices in the face
                v0_pos = line_string.find(" ");
                while (read_buffer[v0_pos] == ' ')
                    v0_pos++;
                // the next occurence of a whitespace (after vertex)
                v1_pos = line_string.find(" ", v0_pos);
                while (read_buffer[v1_pos] == ' ')
                    v1_pos++;
                // string with first vertex
                v0 = line_string.substr(v0_pos, v1_pos - v0_pos);
                // we can tokenise it directly here
                strcpy(buffer, v0.c_str()); // reuse the buffer we used to determine number of vertices
                token = strtok(buffer, "/");
                triangle_data[0] = token ? atoi(token) : 0;
                token = strtok(NULL, "/");
                triangle_data[1] = token ? atoi(token) : 0;
                token = strtok(NULL, "/");
                triangle_data[2] = token ? atoi(token) : 0;

                // the substring containing remainding vertex data (ie minus the first vertex)
                std::string remainder = line_string.substr(v1_pos, line_string.length());
                // loop over the vertices
                for (int i = 0; i < vertices - 2; i++)
                {
                    // we need to compute v1 and v2 positions in the remainder
                    v1_pos = remainder.find(" ");
                    // we also need to remove whitespace
                    while (remainder[v1_pos] == ' ')
                        v1_pos++;
                    v2_pos = remainder.find(" ", v1_pos);
                    // we also need to remove whitespace
                    while (remainder[v2_pos] == ' ')
                        v2_pos++;
                    

                    // now we extract the substrings
                    v1 = remainder.substr(0, v1_pos);
                    v2 = remainder.substr(v1_pos, v2_pos - v1_pos);

                    // update the remainder by shaving off the first vertex
                    remainder = remainder.substr(v1_pos, remainder.length());

                    // we now need to process the vertices we have and create a triangle
                    Triangle* triangle = new Triangle();

                    // tokenise each vertex
                    // v1
                    strcpy(buffer, v1.c_str());
                    token = strtok(buffer, "/");
                    triangle_data[3] = token ? atoi(token) : 0;
                    token = strtok(NULL, "/");
                    triangle_data[4] = token ? atoi(token) : 0;
                    token = strtok(NULL, "/");
                    triangle_data[5] = token ? atoi(token) : 0;

                    // v2
                    strcpy(buffer, v2.c_str());
                    token = strtok(buffer, "/");
                    triangle_data[6] = token ? atoi(token) : 0;
                    token = strtok(NULL, "/");
                    triangle_data[7] = token ? atoi(token) : 0;
                    token = strtok(NULL, "/");
                    triangle_data[8] = token ? atoi(token) : 0;

                    // copy the values into the triangle one vertex at a time
                    for (int j = 0; j < 3; j++)
                    {
                        triangle->positions[j] = triangle_data[j * 3] - 1;
                        // we are missing either texture or normals when the last entry is 0
                        if (triangle_data[j * 3 + 2] == 0)
                        {
                            if (object_properties_ & kHasTextures)
                            {
                                triangle->textures[j] = triangle_data[j * 3 + 1] - 1;
                                triangle->normals[j] = triangle_data[j * 3 + 2];
                            }
                            else if (object_properties_ & kHasNormals)
                            {
                                triangle->textures[j] = triangle_data[j * 3 + 2];
                                triangle->normals[j] = triangle_data[j * 3 + 1] - 1;
                            }
                        }
                        else
                        {
                            triangle->textures[j] = triangle_data[j * 3 + 1] - 1;
                            triangle->normals[j] = triangle_data[j * 3 + 2] - 1;
                        }
                    }
                    triangle->index = current_triangle++;
                    // add current triangle to vector of triangles
                    triangles_.push_back(triangle);
                }
                break;
            }
        }
    }

    // compute centre of gravity and object size
    centre_of_gravity_ = glm::vec3(0.0, 0.0, 0.0);
    // if there are any vertices at all
    if (vertices_.size() != 0)
    {
        // sum up all of the vertex positions
        for (unsigned int vertex = 0; vertex < vertices_.size(); vertex++)
            centre_of_gravity_ += vertices_[vertex];
        // object barycentre
        centre_of_gravity_ = centre_of_gravity_ / (float)vertices_.size();
        // start with 0 radius
        object_size_ = 0.0;
        // now compute the largest distance from the origin to a vertex
        for (unsigned int vertex = 0; vertex < vertices_.size(); vertex++)
        {
            // compute the distance from the barycentre
            float distance = glm::distance(vertices_[vertex], centre_of_gravity_);         
            // now test for maximality
            if (distance > object_size_)
                object_size_ = distance;
        } // per vertex
    }

    // scale all the vertex positions to the desired size
    for (unsigned int vertex = 0; vertex < vertices_.size(); vertex++)
        vertices_[vertex] = (vertices_[vertex] * target_size_) / object_size_;

    // update centre
    centre_of_gravity_ = (centre_of_gravity_ * target_size_) / object_size_;


    // now create the mass points for each vertex
    mass_particles_.resize(vertices_.size());
    for (unsigned int part = 0; part < mass_particles_.size(); part++)
    {
        mass_particles_[part] = new PointMass(cloth_mass_, glm::vec3(0), vertices_[part] + glm::vec3(0, y_pos_, 0));
        mass_particles_[part]->index = part;
    }
    
    // use face triangles to uniquely link point masses together
    unsigned int spring_index = 0;
    // loop through triangle edges 
    for (unsigned int tri = 0; tri < triangles_.size(); tri++)
        for (unsigned int i = 0; i < 3; i++)
            // if there are no springs already linking these points up
            if (CheckPointSprings(mass_particles_[triangles_[tri]->positions[i]], (i + 1) % 3)
                && CheckPointSprings(mass_particles_[triangles_[tri]->positions[(i + 1) % 3]], i))
                {
                    // add the spring to the springs vector
                    springs_.push_back(new Spring(mass_particles_[triangles_[tri]->positions[i]], 
                                                  mass_particles_[triangles_[tri]->positions[(i + 1) % 3]], 
                                                  cloth_k_, cloth_d_));
                    // also add the spring's index to the ball's vector
                    mass_particles_[triangles_[tri]->positions[i]]->spring_indices_.push_back(spring_index++);
                }
    return true;
}

bool ClothObject::CheckPointSprings(PointMass* point_a, unsigned int index_b)
{
    // loop over the point's springs
    for (unsigned int spring = 0; spring < point_a->spring_indices_.size(); spring++)
        // if there is a spring from a to b
        if (springs_[spring]->right_->index == index_b)
            return false;
    return true;
}

bool ClothObject::ReadTexture(std::string &ppm_file)
{
    // line reading buffer
    char read_buffer[MAXIMUM_LINE_LENGTH];

    // open a file stream
    std::ifstream file;
    file.open(ppm_file, std::ios::in);

    // return false if we couldn't open the obj file
    if (!file.is_open())
        return false;

    // first line
    file.getline(read_buffer, MAXIMUM_LINE_LENGTH);
    // should be P3
    if (strcmp(read_buffer,"P3") != 0)
        return false;
    
    // skip comments
    while(file.good() && file.peek() == '#')
        file.getline(read_buffer, MAXIMUM_LINE_LENGTH);

    // read ppm characteristics
    int new_width, new_height, max;
    file >> new_width >> new_height >> max;
    if (max != 255)
        return false;

    // reset texture data
    delete texture_;

    // set new dimensions
    height_ = new_height;
    width_ = new_width;

    // allocate space for texture
    texture_ = new RGB[height_ * width_];

    int temp;
    for (int row = 0; row < height_; row++)
        for (int col = 0; col < width_; col++)
        {
            // read rgb values as integers
            file >> temp;
            // which are then cast as unsigned char (bytes) when assigned to a RGB struct
            texture_[row * height_ + col].red = temp;
            file >> temp;
            texture_[row * height_ + col].green = temp;
            file >> temp;
            texture_[row * height_ + col].blue = temp;
        }
    return true;        
}

void ClothObject::WriteObject(std::string &obj_file)
{
    // open the desired file
    // open a file stream
    std::ofstream file;
    file.open(obj_file, std::ios::out);

    // return if we couldn't open the obj file
    if (!file.is_open())
        return;

    //file << std::fixed << std::setprecision(6) ;

    // start with file name as a comment
    file << "# " << obj_file << '\n';

    // the positions stored in the mass points (NB we omit normals)
    for (unsigned int mass = 0; mass < mass_particles_.size(); mass++)
        file << "v " << mass_particles_[mass]->position_.x 
             << ' ' << mass_particles_[mass]->position_.y 
             << ' ' << mass_particles_[mass]->position_.z << '\n';

    if (object_properties_ & kHasTextures)
        for (unsigned int tex_coord = 0; tex_coord < texture_coords_.size(); tex_coord++)
            file << "vt " << texture_coords_[tex_coord].x 
                << ' ' << texture_coords_[tex_coord].y 
                << ' ' << texture_coords_[tex_coord].z << '\n';

    // the face data as triangles
    for (unsigned int tri = 0; tri < triangles_.size(); tri++)
    {
        file << "f ";
        for (unsigned int v = 0; v < 3; v++)
            {
                file << triangles_[tri]->positions[v] + 1;
                if (object_properties_ & kHasTextures)
                    file << "//" << (int)triangles_[tri]->textures[v] + 1;
                file << ' ';
            }
        file << '\n';
    }

    file.close();
}

//
// OpenGL
//

// sets the OpenGL texture parameters
void ClothObject::SetTexture()
{
    glEnable(GL_TEXTURE_2D);
    glDeleteTextures(1, &texture_id_);
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_);
    glDisable(GL_TEXTURE_2D);
}

void ClothObject::Render()
{
    // set the texture
    if (object_properties_ & kHasTextures)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
    }

    // apply the translation to the centre of the object if requested
    glTranslatef(-centre_of_gravity_.x, -centre_of_gravity_.y, -centre_of_gravity_.z);

    glm::vec3 normal;

    // iterate through triangles and draw vertices with properties 
    glBegin(GL_TRIANGLES);
    for (unsigned int t = 0; t < triangles_.size(); t++)
    {
        // compute the normal of the face
        normal = glm::normalize(glm::cross(
            mass_particles_[triangles_[t]->positions[1]]->position_ - mass_particles_[triangles_[t]->positions[0]]->position_,
            mass_particles_[triangles_[t]->positions[2]]->position_ - mass_particles_[triangles_[t]->positions[0]]->position_));
        
        glNormal3f(normal.x, normal.y, normal.z);

        for (unsigned int v = 0; v < 3; v++)
        {
            // texture uv
            if (object_properties_ & kHasTextures)
                glTexCoord2f(texture_coords_[triangles_[t]->textures[v]].x, 
                             texture_coords_[triangles_[t]->textures[v]].y);
            // position
            glVertex3f(mass_particles_[triangles_[t]->positions[v]]->position_.x, 
                       mass_particles_[triangles_[t]->positions[v]]->position_.y, 
                       mass_particles_[triangles_[t]->positions[v]]->position_.z);
        }
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void ClothObject::ShowPoints()
{
    for (unsigned int p = 0; p < mass_particles_.size(); p++)
            mass_particles_[p]->DrawPoint();
}

// implements flat shading on cloth triangles
void ClothObject::ComputeNormals()
{
    // loop through the triangles and compute the normals of each face
    for (unsigned int tri = 0; tri < triangles_.size(); tri++)
    {
        glm::vec3 normal = glm::normalize(glm::cross(
            mass_particles_[triangles_[tri]->positions[1]]->position_ - mass_particles_[triangles_[tri]->positions[0]]->position_,
            mass_particles_[triangles_[tri]->positions[0]]->position_ - mass_particles_[triangles_[tri]->positions[2]]->position_));
        // update the normals vector
        for (unsigned int i = 0; i < 3; i++)
            normals_[triangles_[tri]->normals[i]] = normal;
    }    
}

//
// Cloth Simulation
//

void ClothObject::ComputeForces(glm::vec3 gravity, glm::vec3 wind, float air_res)
{
    // start by adding external forces (also takes care of resetting the force)
    for (unsigned int p = 0; p < mass_particles_.size(); p++)
        mass_particles_[p]->net_F_ = gravity + wind - (cloth_air_ * mass_particles_[p]->velocity_);

    // update the force of the object's point masses by looping over springs and computing spring force
    for (unsigned int s = 0; s < springs_.size(); s++)
        springs_[s]->UpdateParticles(cloth_k_, cloth_d_);
}

// height and width give the desired cell number
void ClothObject::GenClothGrid(int height, int width, float size)
{
    object_properties_ = 3;

    // the number of vertices along height and width
    int rows = height + 1;
    int cols = width + 1;
    // start in bottom left corner (-x,0,z)
    glm::vec3 bot_left(-size / 2.0, y_pos_, size / 2.0);

    // resize our data
    vertices_.resize(rows * cols);
    normals_.resize(rows * cols);
    texture_coords_.resize(rows * cols);

    // generate the vertices and their data
    for (float row = 0; row < rows; row++)
        for (float col = 0; col < cols; col++)
        {
            vertices_[row * rows + col] = bot_left + glm::vec3(col * (size / width), 0.0, -row * (size / height));
            texture_coords_[row * rows + col] = glm::vec3(col * (1.0 / width), row * (1.0 / height), 0.0);
            normals_[row * rows + col] = glm::vec3(0.0, 1.0, 0.0);
        }
    
    // generate the triangles for rendering (two triangles per cell)
    unsigned int current_triangle = 0;
    triangles_.resize(2 * height * width);
    for (int row = 0; row < height; row++)
        for (int col = 0; col < width; col++)
        {
            // bottom left triangle
            Triangle *triangle_1 = new Triangle();
            triangle_1->positions[0] = triangle_1->normals[0] = triangle_1->textures[0] = row * rows + col;
            triangle_1->positions[1] = triangle_1->normals[1] = triangle_1->textures[1] = row * rows + col + 1;
            triangle_1->positions[2] = triangle_1->normals[2] = triangle_1->textures[2] = (row + 1) * rows + col;
            triangle_1->index = current_triangle++;
            // top right triangle
            Triangle *triangle_2 = new Triangle();
            triangle_2->positions[0] = triangle_2->normals[0] = triangle_2->textures[0] = (row + 1) * rows + col;
            triangle_2->positions[1] = triangle_2->normals[1] = triangle_2->textures[1] = row * rows + col + 1;
            triangle_2->positions[2] = triangle_2->normals[2] = triangle_2->textures[2] = (row + 1) * rows + col + 1;
            triangle_2->index = current_triangle++;
            // put triangles in container
            unsigned int tri = triangle_1->index;
            triangles_[triangle_1->index] = triangle_1;
            triangles_[triangle_2->index] = triangle_2;
        }
    
    // as many mass points as vertices
    mass_particles_.resize(vertices_.size());
    // now create the mass points for each vertex
    mass_particles_.resize(vertices_.size());
    for (unsigned int part = 0; part < mass_particles_.size(); part++)
    {
        mass_particles_[part] = new PointMass(cloth_mass_, glm::vec3(0), vertices_[part]);
        mass_particles_[part]->index = part;
    }

    //for (int i = 0; i < vertices_.size(); i++)
        //std::cout << vertices_[i].x << " " << vertices_[i].y << " " << vertices_[i].z << std::endl;
    
    // 2 springs per cell and m(n+1) + m(n+1) springs for each unique edges for n x m cells cloth
    unsigned int spring_index = 0;
    unsigned int left, right = 0;
    springs_.resize(0);// 2 * height * width + width * (height + 1) + height * (width + 1));
    for (unsigned int row = 0; row < height; row++)
        for (unsigned int col = 0; col < width; col++)
        {
            //      c--------d
            //      |        |
            //      |        |
            //      |        |
            //      a--------b
            //
            // We can add the springs a-d, c-b, c-d, b-d without checking if a spring
            // already exists, however we have to check a-c and a-b because we are iterating
            // row-wise starting from the bottom left of the cloth grid

            // spring a-b
            left = row * rows + col;
            right = row * rows + col + 1;
            if (CheckPointSprings(mass_particles_[left], right)
                && CheckPointSprings(mass_particles_[right], left))
                {
                    springs_.push_back(new Spring(mass_particles_[left], mass_particles_[right], cloth_k_, cloth_d_));
                    mass_particles_[left]->spring_indices_.push_back(spring_index++);
                }

            // spring a-c
            right = (row + 1) * rows + col;
            if (CheckPointSprings(mass_particles_[left], right)
                && CheckPointSprings(mass_particles_[right], left))
                {
                    springs_.push_back(new Spring(mass_particles_[left], mass_particles_[right], cloth_k_, cloth_d_));
                    mass_particles_[left]->spring_indices_.push_back(spring_index++);
                }

            // spring a-d
            right = (row + 1) * rows + col + 1;
            springs_.push_back(new Spring(mass_particles_[left], mass_particles_[right], cloth_k_, cloth_d_));
            mass_particles_[left]->spring_indices_.push_back(spring_index++);

            // spring b-d
            left = row * rows + col + 1;
            right = (row + 1) * rows + col + 1;
            springs_.push_back(new Spring(mass_particles_[left], mass_particles_[right], cloth_k_, cloth_d_));
            mass_particles_[left]->spring_indices_.push_back(spring_index++);

            // spring c-b
            left = (row + 1) * rows + col;
            right = row * rows + col + 1;
            springs_.push_back(new Spring(mass_particles_[left], mass_particles_[right], cloth_k_, cloth_d_));
            mass_particles_[left]->spring_indices_.push_back(spring_index++);

            // spring c-d
            left = (row + 1) * rows + col;
            right = (row + 1) * rows + col + 1;
            springs_.push_back(new Spring(mass_particles_[left], mass_particles_[right], cloth_k_, cloth_d_));
            mass_particles_[left]->spring_indices_.push_back(spring_index++);
        }
}

// 
// DEBUG
//
