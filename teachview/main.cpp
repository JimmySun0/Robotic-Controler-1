#include "mainwindow.h"
#include <QApplication>
#include <Threads/ReadPipeThread.h>
#include <QObject>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    ReadpipeThread pipethread;
    QObject::connect(&w, SIGNAL(windowClosed()), &pipethread, SLOT(terminate()));
    QObject::connect(&w, SIGNAL(windowClosed()), &pipethread, SLOT(quit()));
    w.show();
    return a.exec();
}
