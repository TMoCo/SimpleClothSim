// Where the main window is created 

// window declaration
#include "Window.h"

// the QApplication
#include <QApplication>

int main(int argc, char **argv)
{
    // create a Qt application
    QApplication app(argc, argv);

    // create the root widget
    Window theWindow(NULL);

    // resize it
    theWindow.resize(1024,768);

    // display the widget
    theWindow.show();

    // execute the Qt application
    return app.exec();
}