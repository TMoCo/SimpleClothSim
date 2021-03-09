#ifndef WINDOW_H
#define WINDOW_H

// standard Qt widgets
#include <QtWidgets>

// the simulation widget
#include "SimulationWidget.h"

// convenience class for animation control buttons
class CtrlButton : public QPushButton
{
    Q_OBJECT
    public:
    //  constructor, sets the geometry of button 
    CtrlButton(QString label, QWidget *parent);
};

// Window class that behaves as a root widget for the application
class Window : public QWidget
{
    Q_OBJECT
    public:
    // constructor
    Window(QWidget* parent);
    // destructor
    ~Window();

    public slots:
    void OpenObjDialog();
    void OpenPpmDialog();
    void SaveObjDialog();
    void SetGravitySlider(QAbstractButton* box_clicked);
    void SetIntegrationMethod(QAbstractButton* box_clicked);

    // signals for file loading and saving
    signals:
    void SelectedReadObj(QString file_name);
    void SelectedReadPpm(QString file_name);
    void SelectedWriteObj(QString file_name);

    private:

    // window layout
    // main layout
    QVBoxLayout* main_layout_;
    // sub layout to have simulation layouts to the left of controls
    QHBoxLayout* sub_layout_;
    // layout for simulation widgets (openGL widget and player)
    QVBoxLayout* simulation_layout_;
    // layout for controls (cloth, simulation properties like gravity &c...)
    QVBoxLayout* controls_layout_;

    //
    // MENU WIDGETS
    //
    QMenuBar* menu_bar_;

    // widgets for loading and saving .obj (geometry) and .ppm (textures)
    QMenu* file_menu_;
    QAction* open_obj_;
    QAction* open_ppm_;
    QAction* save_obj_;

    // widgets for changing scenes
    QMenu* scene_menu_;
    QAction* default_;
    QAction* scenario_one_;
    QAction* scenario_two_;

    //
    // SIMULATION WIDGETS & PLAYBACK
    //

    // widget where the simulation is rendered
    SimulationWidget* simulator_;

    // timer for updating the scene
    QTimer* timer_;

    // layout for the simulation
    QGridLayout* player_layout_;

    // widgets for controlling the simulation
    CtrlButton* play_;
    CtrlButton* stop_;
    CtrlButton* reset_;

    //
    // SIMULATION PROPERTIES EDITOR
    //

    // container for cloth properties
    QGroupBox* cloth_group_;
    QGridLayout* cloth_layout_;
    QLabel* mass_label_;
    QSlider* mass_slider_;
    QLabel* stiff_label_;
    QSlider* stiff_slider_;
    QLabel* damp_label_;
    QSlider* damp_slider_;
    QCheckBox* show_mass_;
    // container for simulation properties
    QGroupBox* properties_group_;
    QVBoxLayout* properties_layout_;
    QLabel* gravity_label_;
    QSlider* gravity_slider_;
    QGroupBox* gravity_group_;
    QHBoxLayout* gravity_layout_;
    QButtonGroup* gravity_boxes_;
    QCheckBox* earth_;
    QCheckBox* moon_;
    QCheckBox* mars_;
    QLabel* wind_label_;
    QSlider* wind_slider_;
    QLabel* air_label_;
    QSlider* air_slider_;
    QLabel* static_label_;
    QSlider* static_slider_;
    QLabel* kinetic_label_;
    QSlider* kinetic_slider_;
    // container for integration scheme
    QGroupBox* integration_group_;
    QButtonGroup* integration_boxes_;
    QVBoxLayout* integration_layout_;
    QCheckBox* exp_Euler_;
    QCheckBox* imp_Euler_;
};

#endif