#ifndef SIMULATION_WIDGET
#define SIMULATION_WIDGET

// for Qt
#include <QGLWidget>
#include <QMouseEvent>

// the ball in the scene
#include "Ball.h"
// the spring tying the ball
#include "Spring.h"
// the object loaded from an Obj file
#include "ClothObject.h"
// the collidable objects
#include "Collidable.h"

class SimulationWidget : public QGLWidget
{
    public:
    // enums to determine current scene type and how clothObject should be handled
    enum Scene : unsigned int
    {
        kDefault = 0,
        kScenarioOne = 1,
        kScenarioTwo = 2
    };

    // enums to determine current integration
    enum Integration : unsigned int
    {
        kExplicitEuler = 0,
        kImplicitEuler = 1
    };

    Q_OBJECT

    public slots:
    // integration slots for updating the cloth, called by timer every 1/60 seconds
    void UpdateObjects();
    // file I/O slots
    void ReadObjFile(QString file_name);
    void ReadPpmFile(QString file_name);
    void WriteObjFile(QString file_name);
    // display slots
    void ShowPoints(int state);
    void ResetSimulation();
    // cloth slots
    void UpdateMass(int new_mass);
    void UpdateStiffness(int new_k);
    void UpdateDampening(int new_d);
    // simulation slots
    void UpdateGravity(int new_gravity);
    void UpdateAirResistance(int new_air);
    void UpdateWind(int new_wind);
    void UpdateStatic(int new_static);
    void UpdateKinetic(int new_kinetic);
    // scene setting
    void SetDefaultScene();
    void SetSceneOne();
    void SetSceneTwo();

    public:
    // constructor
    SimulationWidget(QWidget* parent);
    // destructor
    ~SimulationWidget();


    // Qt opengl functions
	// called when OpenGL context is set up
	void initializeGL();
	// called every time the widget is resized
	void resizeGL(int w, int h);
	// called every time the widget needs painting
	void paintGL();

    // integration
    void StepExplicitEuler();
    void StepImplicitEuler();

    // mouse input
    HVect mouseToWorld(float mouseX, float mouseY);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void TransformWind(float matrix[16]);

    // the object in the scene
    ClothObject* object_;

    // the collidable objects in the scene
    Collidable **collidables_;
    unsigned int n_collidables_;

    // the current scene, dictates how some object behave
    Scene current_scene_;

    // integration method selected
    Integration method_;

    // arc ball data
    BallData arc_ball_;
    int button_pressed_;
    HVect ball_centre_;

    // the time step delta t in seconds
    float delta_time_;
    float air_resistance_;
    float gravity_;
    float kinetic_;
    float static_;
    float wind_;
    glm::vec3 wind_dir_;

    // flag for showing an object's mass points as spheres
    int show_points_;

    // arbitrary size for view space
    float size_;
};


#endif