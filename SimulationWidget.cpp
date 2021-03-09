// the widget declaration
#include <SimulationWidget.h>

// file stream 
#include <fstream>

// opengGL functions
#include <GL/gl.h>
#include <GL/glu.h>

// making a matrix from an array of values
#include <glm/gtc/type_ptr.hpp>

// directional light along z axis
static float light_position[] = {1.0, 1.0, 1.0, 0.0};	

// constructor
SimulationWidget::SimulationWidget(QWidget* parent) : QGLWidget(parent)
{
    // initialise the pointer to the cloth object
    object_ = new ClothObject();
    // and to the collidables
    collidables_ = NULL;
    n_collidables_ = 0;
    // simulation parameters
    delta_time_ = 0.0016;

    // tell qt to enable mouse tracking
    setMouseTracking(true);
    
    // init state
    show_points_ = 0;
    current_scene_ = kDefault;
    method_ = kExplicitEuler;
    SetDefaultScene();
    size_ = 2.0;
    wind_ = 0;
    wind_dir_= glm::vec3(0, 1, 0);

    // init arc ball (take into account the scene transform to be in view)
    Ball_Init(&arc_ball_);
    Ball_Place(&arc_ball_, qOne , size_);
    button_pressed_ = -1;
}

// destructor
SimulationWidget::~SimulationWidget()
{
    // do something
}

//
// Integration slots
//

void SimulationWidget::UpdateObjects()
{ 
    switch (method_)
    {
        case (kExplicitEuler):
            StepExplicitEuler();
            break;
        case (kImplicitEuler):
            StepImplicitEuler();
            break;
        
        default:
            break;
    }  
    updateGL();
}

//
// I/O Slots
//

void SimulationWidget::ReadObjFile(QString file_name)
{
    std::string obj = file_name.toStdString();
    object_->y_pos_ = 1.5 * size_; 
    object_->ReadObject(obj);
}

void SimulationWidget::ReadPpmFile(QString file_name)
{
    std::string ppm = file_name.toStdString();
    object_->ReadTexture(ppm);
    object_->SetTexture();
}

void SimulationWidget::WriteObjFile(QString file_name)
{
    std::string obj = file_name.toStdString();
    object_->WriteObject(obj);
}

//
// Display Slots
//

void SimulationWidget::ResetSimulation()
{
    // reset the properties of the simulation to default ie
    for (unsigned int p = 0; p < object_->mass_particles_.size(); p++)
    {
        // velocity of zero
        object_->mass_particles_[p]->velocity_ = glm::vec3(0);
        // initial vertex positions
        object_->mass_particles_[p]->position_ = object_->vertices_[p] + glm::vec3(0, object_->y_pos_, 0);
    }
    updateGL();
}

void SimulationWidget::ShowPoints(int state)
{
    show_points_ = state;
    updateGL();
}

//
// Cloth Slots 
//

void SimulationWidget::UpdateMass(int new_mass)
{
    object_->cloth_mass_ = new_mass / 10.0;
}

void SimulationWidget::UpdateStiffness(int new_k)
{
    object_->cloth_k_ = new_k * 100.0;
}

void SimulationWidget::UpdateDampening(int new_d)
{
    object_->cloth_d_ = new_d;
}

//
// Simulation Slots
//

void SimulationWidget::UpdateGravity(int new_gravity)
{
    gravity_ = new_gravity / 10.0;
}

void SimulationWidget::UpdateAirResistance(int new_air)
{
    air_resistance_ = new_air / 50.0;
}

void SimulationWidget::UpdateWind(int new_wind)
{
    wind_ = new_wind / 10.0;
}

void SimulationWidget::UpdateStatic(int new_static)
{
    static_ = new_static / 10.0;
    collidables_[0]->static_friction_ = static_;
}

void SimulationWidget::UpdateKinetic(int new_kinetic)
{
    kinetic_ = new_kinetic / 10.0;
    collidables_[0]->kinetic_friction_ = kinetic_;
}


//
// Scene Slots (also setup)
//

// will place a floor in the scene
void SimulationWidget::SetDefaultScene()
{
    current_scene_ = kDefault;
    object_->y_pos_ = 1.5 * size_;
    // remove previous collidables and objects
    object_->ClearObject();
    delete collidables_;
    // create collidables
    n_collidables_ = 1;
    collidables_ = new Collidable*[n_collidables_];
    // collidable is a Floor
    collidables_[0] = new Floor(static_, kinetic_, 2.0 * size_, glm::vec3(0.0));
    ResetSimulation();
}

// will place a floor and a sphere in the scene
void SimulationWidget::SetSceneOne()
{
    object_->ClearObject();
    current_scene_ = kScenarioOne;
    object_->y_pos_ = 0.75 * size_;
    // generate a cloth grid (removes previous object)
    object_->GenClothGrid(50, 50, 1.5 * size_);
    // remove collidables of the previous scene
    delete collidables_;
    // create two collidables
    n_collidables_ = 2;
    collidables_ = new Collidable*[n_collidables_];
    // place a floor in the scene and a ball
    collidables_[0] = new Floor(static_, kinetic_, 2.0 * size_, glm::vec3(0.0));
    collidables_[1] = new Sphere(static_, kinetic_, size_ / 4.0, glm::vec3(0.0, size_ / 4.0, 0.0));
    // update the scene
    ResetSimulation();
}

// will place a floor in the scene
void SimulationWidget::SetSceneTwo()
{
    object_->ClearObject();
    current_scene_ = kScenarioTwo;
    object_->y_pos_ = 0.75 * size_;
    // generate a cloth grid (removes previous object)
    object_->GenClothGrid(30, 30, 1.5 * size_);
    // remove previous collidables
    delete collidables_;
    // create a collidable
    n_collidables_ = 1;
    collidables_ = new Collidable*[n_collidables_];
    // create a floor object
    collidables_[0] = new Floor(static_, kinetic_, 2.0 * size_, glm::vec3(0.0));
    // update the scene
    ResetSimulation();
}

//
// OPENGL METHODS
//

void SimulationWidget::initializeGL()
{
    // enable Z-buffering
    glEnable(GL_DEPTH_TEST);

    // set lighting parameters
    glShadeModel(GL_FLAT);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    // background is pink
    glClearColor(0.3, 0.0, 0.6, 1.0);
}

// called every time the widget is resized
void SimulationWidget::resizeGL(int w, int h)
{
    // set viewport to width and height
    glViewport(0, 0, w, h);

    // set projection matrix to be glOrtho based on zoom & window size
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // stick with orthogonal projection
    float aspectRatio = (float) w / (float) h;

    // project according to aspect ratio
    if (aspectRatio > 1.0)
		glFrustum(-aspectRatio * size_, aspectRatio * size_, -size_, size_, 1, 200);
	else
        glFrustum(-size_, size_, -size_/aspectRatio, size_/aspectRatio, 1, 200);
	
    
}
	
// called every time the widget needs painting
void SimulationWidget::paintGL()
{
    // set up scene parameters
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_COLOR_MATERIAL);

    // load identity in modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // view scene from above
    glRotatef(45, 1, 0, 0);
    // move the scene into view
    glTranslatef(0, 2.0 * -size_, -size_);


    // draw a line for the wind
    glPushMatrix();
    glColor3f(1, 0, 0);
    // top left of screen
    glTranslatef(size_ , 2 * size_ , -0.3);
    GLfloat mNow[16];
    Ball_Value(&arc_ball_, mNow);
	glMultMatrixf(mNow);
    // a small line
    glBegin(GL_LINES);
    glVertex3f(0.3, 0, 0);
    glVertex3f(-0.3, 0, 0);
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.2, 0.1, 0);
    glVertex3f(0.2, -0.1, 0);
    glVertex3f(0.3, 0, 0);
    glVertex3f(-0.32, 0.1, 0);
    glVertex3f(-0.32, -0.1, 0);
    glVertex3f(-0.3, 0, 0);
    glEnd();
    glPopMatrix();

    glPolygonMode(GL_FRONT, GL_TRIANGLES);


    glEnable(GL_LIGHTING);
    // set light position at desired position
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // a default color
    glColor3f(1, 1, 1);

    // render the collidable objects
    for (unsigned int obj = 0; obj < n_collidables_; obj++)
        collidables_[obj]->DrawCollidable();

    glPushMatrix();
    // centre the object
    glTranslatef(-object_->centre_of_gravity_.x, -object_->centre_of_gravity_.y, -object_->centre_of_gravity_.z);
    object_->Render();
    if (show_points_)
        object_->ShowPoints();
    glPopMatrix();
    
}

//
// Integration
//

void SimulationWidget::StepExplicitEuler()
{
    unsigned int first_particle = 0;
    unsigned int last_particle = object_->mass_particles_.size();

    if (current_scene_ == kScenarioTwo)
    {
        first_particle++;
        last_particle--;
    }

    // step 1 compute forces
    object_->ComputeForces(glm::vec3(0.0, -gravity_, 0.0), wind_ * wind_dir_, air_resistance_);
    
    // step 2 check collisions with collidables
    for (unsigned int obj = 0; obj < n_collidables_; obj++)
        for (unsigned int point = 0; point < object_->mass_particles_.size(); point++)
            collidables_[obj]->ComputeCollision(object_->mass_particles_[point], object_->cloth_gravity_);
    
    // loop over particles
    for (unsigned int particle = first_particle; particle < last_particle; particle++)
    {
        // step 3 update positions
        object_->mass_particles_[particle]->position_ += object_->mass_particles_[particle]->velocity_ * delta_time_;
        // step 4 update velocities
        object_->mass_particles_[particle]->velocity_ += (object_->mass_particles_[particle]->net_F_ / object_->cloth_mass_) * delta_time_;

    }
}

void SimulationWidget::StepImplicitEuler()
{
    unsigned int first_particle = 0;
    unsigned int last_particle = object_->mass_particles_.size();

    if (current_scene_ == kScenarioTwo)
    {
        first_particle++;
        last_particle--;
    }
    // step 1 compute forces
    object_->ComputeForces(glm::vec3(0.0, -gravity_, 0.0), wind_ * wind_dir_, air_resistance_);

    // step 2 check collisions with collidables
    for (unsigned int obj = 0; obj < n_collidables_; obj++)
        for (unsigned int point = 0; point < object_->mass_particles_.size(); point++)
            collidables_[obj]->ComputeCollision(object_->mass_particles_[point], object_->cloth_gravity_);

    // loop over particles
    for (unsigned int particle = first_particle; particle < last_particle; particle++)
    {
        // step 3 update the velocities
        object_->mass_particles_[particle]->velocity_ += (object_->mass_particles_[particle]->net_F_ / object_->cloth_mass_) * delta_time_;
        // step 4 update the positions
        object_->mass_particles_[particle]->position_ += object_->mass_particles_[particle]->velocity_ * delta_time_;
    }    
}


//
// Mouse input
// 

void SimulationWidget::mousePressEvent(QMouseEvent *event)
{
    HVect curr_mouse = mouseToWorld(event->localPos().x(), event->localPos().y());
    button_pressed_ = event->button();
    switch (button_pressed_)
    {
        case(Qt::LeftButton):
            Ball_Mouse(&arc_ball_, curr_mouse);
			Ball_BeginDrag(&arc_ball_);
            updateGL();
            break;
        default:
            break;
    }
}

void SimulationWidget::mouseMoveEvent(QMouseEvent *event)
{
    HVect curr_mouse = mouseToWorld(event->localPos().x(), event->localPos().y());
    switch (button_pressed_)
    {
        case(Qt::LeftButton):
            Ball_Mouse(&arc_ball_, curr_mouse);
			Ball_Update(&arc_ball_);
            // get the arcball rotation as a column major matrix
            float rotation[16];
            Ball_Value(&arc_ball_, rotation);
            // rotate the wind
            TransformWind(rotation);
            updateGL();
            break;
        default:
            break;
    }
}

void SimulationWidget::mouseReleaseEvent(QMouseEvent *event)
{
    switch (button_pressed_)
    {
        case(Qt::LeftButton):
            Ball_EndDrag(&arc_ball_);
            button_pressed_ = -1;
            updateGL();
            break;
        default:
            break;
    }
}

HVect SimulationWidget::mouseToWorld(float mouseX, float mouseY)
{
    HVect worldMouse;
    // transorm back into world coordinates, qt mouse event has origin in top left of widget vs bottom left for OpenGL
    worldMouse.x = (2.0 * mouseX / width() - 1) * size_; // multiply by size?;
    worldMouse.y = (-2.0 * mouseY / height() + 1) * size_; // multiply by size?;
    worldMouse.z = 0;
    // compute aspect ratio
    float aspectRatio = (float) width() / (float) height();
    // accomodate for aspect ratio
    if (aspectRatio > 1.0)
		worldMouse.x *= aspectRatio;
	else
        worldMouse.y *= aspectRatio;
    return worldMouse;
}

void SimulationWidget::TransformWind(float matrix[16])
{
    // get the transform (will always be a rotation)
    glm::mat4 transform = glm::make_mat4(matrix);
    // apply rotation (convert to vec4, apply rotation, convert back to vec3)
    wind_dir_ = glm::vec3(transform * glm::vec4(wind_dir_, 0.0));
}