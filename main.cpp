#include "mainwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget w;

    w.setWindowTitle("Planar Model");
    w.setFixedSize(WINDOW_WIDTH,WINDOW_HEIGHT);
    w.show();

    return a.exec();
}
