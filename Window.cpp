// the window class declaration
#include "Window.h"

// constructor for control button
CtrlButton::CtrlButton(QString label, QWidget *parent) : QPushButton(label, parent)
{
    setFixedSize(70, 30);
}

// constructor
Window::Window(QWidget* parent) : QWidget(parent)
{
    // give the window a good name
    setWindowTitle("Cloth Simulator");

    //
    // SET UP THE WIDGETS
    //

    // create the simulation widget and set the window as its parent
    simulator_ = new SimulationWidget(this);
    simulator_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    // create the timer widget and connect it
    timer_ = new QTimer(this);
    timer_->setInterval(16); // 1 / 60 seconds
    // upon timeout, update the scene
    QObject::connect(timer_, SIGNAL(timeout()), simulator_, SLOT(UpdateObjects())); 


    // create file menu
    menu_bar_ = new QMenuBar(this);
    menu_bar_->setMaximumHeight(25);
    file_menu_ = menu_bar_->addMenu(tr("&File"));
    // create file actions accessed through file menu
    open_obj_ = new QAction(tr("&Open .obj"));
    open_ppm_ = new QAction(tr("&Open .ppm"));
    save_obj_ = new QAction(tr("&Save .obj"));
    // connect to file IO
    QObject::connect(open_obj_, SIGNAL(triggered()), this, SLOT(OpenObjDialog()));
    QObject::connect(this, SIGNAL(SelectedReadObj(QString)), simulator_, SLOT(ReadObjFile(QString)));
    QObject::connect(open_ppm_, SIGNAL(triggered()), this, SLOT(OpenPpmDialog()));
    QObject::connect(this, SIGNAL(SelectedReadPpm(QString)), simulator_, SLOT(ReadPpmFile(QString)));
    QObject::connect(save_obj_, SIGNAL(triggered()), this, SLOT(SaveObjDialog()));
    QObject::connect(this, SIGNAL(SelectedWriteObj(QString)), simulator_, SLOT(WriteObjFile(QString)));
    // add to menu
    file_menu_->addAction(open_obj_);
    file_menu_->addAction(open_ppm_);
    file_menu_->addAction(save_obj_);

    // create scene menu
    scene_menu_ = menu_bar_->addMenu(tr("&Scene"));
    default_ = new QAction(tr("&Default"));
    scenario_one_ = new QAction(tr("&Scenario 1"));
    scenario_two_ = new QAction(tr("&Scenario 2"));
    // connect to simulation widget
    QObject::connect(default_, SIGNAL(triggered()), simulator_, SLOT(SetDefaultScene()));
    QObject::connect(scenario_one_, SIGNAL(triggered()), simulator_, SLOT(SetSceneOne()));
    QObject::connect(scenario_two_, SIGNAL(triggered()), simulator_, SLOT(SetSceneTwo()));
    // add to menu
    scene_menu_->addAction(default_);
    scene_menu_->addAction(scenario_one_);
    scene_menu_->addAction(scenario_two_);


    // init simulation play controls
    player_layout_ = new QGridLayout;
    // init simulation buttons
    play_ = new CtrlButton("&play", this);
    stop_ = new CtrlButton("&stop", this);
    reset_ = new CtrlButton("&reset", this);
    // connect to timer
    QObject::connect(play_, SIGNAL(pressed()), timer_, SLOT(start()));
    QObject::connect(stop_, SIGNAL(pressed()), timer_, SLOT(stop()));
    QObject::connect(reset_, SIGNAL(pressed()), timer_, SLOT(stop()));
    QObject::connect(reset_, SIGNAL(pressed()), simulator_, SLOT(ResetSimulation()));
    // add to the control layout
    player_layout_->addWidget(play_, 0, 0);
    player_layout_->addWidget(stop_, 0, 1);
    player_layout_->addWidget(reset_, 0, 2);


    // init the cloth properties controller
    cloth_group_ = new QGroupBox(tr("&Cloth settings"));
    cloth_layout_ = new QGridLayout;
    mass_label_ = new QLabel(tr("mass"), this);
    mass_slider_ = new QSlider(Qt::Horizontal, this);
    stiff_label_ = new QLabel(tr("stiffness"), this);
    stiff_slider_ = new QSlider(Qt::Horizontal, this);
    damp_label_ = new QLabel(tr("dampening"), this);
    damp_slider_ = new QSlider(Qt::Horizontal, this);
    show_mass_ = new QCheckBox(tr("&show mass points"));
    // connect widgets
    QObject::connect(mass_slider_, SIGNAL(valueChanged(int)), simulator_, SLOT(UpdateMass(int)));
    QObject::connect(stiff_slider_, SIGNAL(valueChanged(int)), simulator_, SLOT(UpdateStiffness(int)));
    QObject::connect(damp_slider_, SIGNAL(valueChanged(int)), simulator_, SLOT(UpdateDampening(int)));
    QObject::connect(show_mass_, SIGNAL(stateChanged(int)), simulator_, SLOT(ShowPoints(int)));
    // set widget settings
    cloth_group_->setMaximumWidth(300);
    mass_slider_->setRange(10, 100);
    mass_slider_->setValue(10);
    stiff_slider_->setRange(0, 300);
    stiff_slider_->setValue(100);
    damp_slider_->setRange(0, 100);
    damp_slider_->setValue(10);
    // place them in the layout
    cloth_layout_->addWidget(mass_label_, 0, 0);
    cloth_layout_->addWidget(mass_slider_, 0, 1);
    cloth_layout_->addWidget(stiff_label_, 1, 0);
    cloth_layout_->addWidget(stiff_slider_, 1, 1);
    cloth_layout_->addWidget(damp_label_, 2, 0);
    cloth_layout_->addWidget(damp_slider_, 2, 1);
    cloth_layout_->addWidget(show_mass_, 3, 0);
    // set the box's layout
    cloth_group_->setLayout(cloth_layout_);


    // init the simulation properties controller
    properties_group_ = new QGroupBox(tr("&Simulation settings"));
    properties_layout_ = new QVBoxLayout;
    //gravity_group_ = new QGroupBox;
    gravity_label_ = new QLabel(tr("gravity"), this);
    gravity_slider_ = new QSlider(Qt::Horizontal, this);
    gravity_layout_ = new QHBoxLayout;
    gravity_boxes_ = new QButtonGroup;
    earth_ = new QCheckBox(tr("&earth"));
    moon_ = new QCheckBox(tr("&moon"));
    mars_ = new QCheckBox(tr("&mars"));
    wind_label_ = new QLabel(tr("wind"), this);
    wind_slider_ = new QSlider(Qt::Horizontal, this);
    air_label_  = new QLabel(tr("air resistance"), this);
    air_slider_ = new QSlider(Qt::Horizontal, this);
    static_label_ = new QLabel(tr("static resistance"), this);
    static_slider_ = new QSlider(Qt::Horizontal, this);
    kinetic_label_ = new QLabel(tr("kinetic friction"), this);;
    kinetic_slider_ = new QSlider(Qt::Horizontal, this);
    // connect the widgets
    QObject::connect(gravity_slider_, SIGNAL(valueChanged(int)), simulator_, SLOT(UpdateGravity(int)));
    QObject::connect(air_slider_, SIGNAL(valueChanged(int)), simulator_, SLOT(UpdateAirResistance(int)));
    QObject::connect(wind_slider_, SIGNAL(valueChanged(int)), simulator_, SLOT(UpdateWind(int)));
    QObject::connect(static_slider_, SIGNAL(valueChanged(int)), simulator_, SLOT(UpdateStatic(int)));
    QObject::connect(kinetic_slider_, SIGNAL(valueChanged(int)), simulator_, SLOT(UpdateKinetic(int)));
    QObject::connect(gravity_boxes_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(SetGravitySlider(QAbstractButton*)));
    // set widget initial settings
    properties_group_->setMaximumWidth(300);
    gravity_boxes_->addButton(earth_, 0); // int is button id
    gravity_boxes_->addButton(moon_, 1);
    gravity_boxes_->addButton(mars_, 2);
    gravity_boxes_->setExclusive(true);
    earth_->setCheckState(Qt::Checked);
    gravity_slider_->setRange(0, 100);
    gravity_slider_->setValue(98);
    wind_slider_->setRange(0, 100);
    wind_slider_->setValue(0);
    air_slider_->setRange(0, 100);
    air_slider_->setValue(0);
    static_slider_->setRange(0, 200);
    static_slider_->setValue(30);
    kinetic_slider_->setRange(0, 200);
    kinetic_slider_->setValue(10);
    // place them in the layout
    gravity_layout_->addWidget(earth_);
    gravity_layout_->addWidget(moon_);
    gravity_layout_->addWidget(mars_);
    properties_layout_->addWidget(gravity_label_);
    properties_layout_->addWidget(gravity_slider_);
    properties_layout_->addLayout(gravity_layout_);
    properties_layout_->addWidget(wind_label_);
    properties_layout_->addWidget(wind_slider_);
    properties_layout_->addWidget(air_label_);
    properties_layout_->addWidget(air_slider_);
    properties_layout_->addWidget(static_label_);
    properties_layout_->addWidget(static_slider_);
    properties_layout_->addWidget(kinetic_label_);
    properties_layout_->addWidget(kinetic_slider_);
    // set the box's layout
    properties_group_->setLayout(properties_layout_);


    // init the integration scheme selector
    integration_group_ = new QGroupBox(tr("&Integration scheme"));
    integration_layout_ = new QVBoxLayout;
    integration_boxes_ = new QButtonGroup;
    exp_Euler_ = new QCheckBox(tr("&explicit Euler")); 
    imp_Euler_ = new QCheckBox(tr("&implicit Euler"));
    // connect the widgets

    // set widget settings
    integration_group_->setMaximumWidth(300);
    integration_boxes_->addButton(exp_Euler_, 0);// int is button id
    integration_boxes_->addButton(imp_Euler_, 1);
    integration_boxes_->setExclusive(true);
    imp_Euler_->setCheckState(Qt::Checked);
    // connect checkboxes
    QObject::connect(integration_boxes_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(SetIntegrationMethod(QAbstractButton*)));
    // add to the layout
    integration_layout_->addWidget(exp_Euler_);
    integration_layout_->addWidget(imp_Euler_);
    // set the box's layout
    integration_group_->setLayout(integration_layout_);

    //
    // SET UP THE WINDOW'S LAYOUT
    // 

    // init the layouts
    main_layout_ = new QVBoxLayout(this);
    sub_layout_ = new QHBoxLayout;
    simulation_layout_ = new QVBoxLayout;
    controls_layout_ = new QVBoxLayout;
    // add the menu bar to the layout
    main_layout_->addWidget(menu_bar_);
    // add the sub layouts
    main_layout_->addLayout(sub_layout_);
    sub_layout_->addLayout(simulation_layout_);
    sub_layout_->addLayout(controls_layout_);
    // add the simulation widget to the simulation layout
    simulation_layout_->addWidget(simulator_);
    // add the simulation controls to the layout
    simulation_layout_->addLayout(player_layout_);
    // add the controls to the controls layout
    controls_layout_->addWidget(cloth_group_);
    controls_layout_->addWidget(properties_group_);
    controls_layout_->addWidget(integration_group_);
}

// destructor
Window::~Window()
{
    // do something
}

// open file browser dialog and emit file name on succesful choice
void Window::OpenObjDialog()
{
    // open dialog
    QString file_name = QFileDialog::getOpenFileName(this, tr("&Load .obj"), "./", tr(".obj (*.obj)"));
    // check name chosen is not empty
    if (!file_name.isEmpty())
        emit SelectedReadObj(file_name);
}

void Window::OpenPpmDialog()
{
    // open dialog
    QString file_name = QFileDialog::getOpenFileName(this, tr("&Load .ppm"), "./", tr(".ppm (*.ppm)"));
    // check name chosen is not empty
    if (!file_name.isEmpty())
        emit SelectedReadPpm(file_name);
}

void Window::SaveObjDialog()
{
    // open dialog
    QString file_name = QFileDialog::getSaveFileName(this, tr("Load .obj"), "./", tr(".obj (*.obj)"));
    // check if file name is valid
    if (!file_name.isEmpty())
    {
        // get the file's info
        QFileInfo file_info(file_name);
        // set the suffix of the file to ".obj"
        // user entered just a file name (no extensions)
        if (file_info.completeSuffix().isEmpty())
            file_name.append(".obj");
        // user entered an extension that is not .obj
        else if (file_info.completeSuffix() != QString("obj"))
            file_name.replace(file_info.completeSuffix(), QString("obj"));
        // emit success
        emit SelectedWriteObj(file_name);     
    }
}

void Window::SetGravitySlider(QAbstractButton* box_clicked)
{
    // call the setValue slider slot, which will update the gavity applied on the cloth
    switch (gravity_boxes_->id(box_clicked))
    {
        case (0):
            gravity_slider_->setValue(98);
            break;
        case (1):
            gravity_slider_->setValue(16);
            break;
        case (2):
            gravity_slider_->setValue(37);
            break;
    }
}

void Window::SetIntegrationMethod(QAbstractButton* box_clicked)
{
    // call the setValue slider slot, which will update the integration method used in the simulation
    switch (integration_boxes_->id(box_clicked))
    {
        // explicit euler
        case (0):
            simulator_->method_ = SimulationWidget::kExplicitEuler;
            break;
        // implicit euler
        case (1):
            simulator_->method_ = SimulationWidget::kImplicitEuler;
            break;
    }
}

