#include "main_window/main_window.h"
#include <QApplication>

int main(int argc, char* argv[]) 
{
    QApplication app(argc, argv);
    
    ImageViewer viewer;
    viewer.setFixedSize(800, 600);
    viewer.show();
    
    return app.exec();
}