#include "mainwindow.h"
#include "gamescene.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    gamescene g;
    g.show();

    return a.exec();
}
