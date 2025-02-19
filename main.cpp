#include "mainwindow.h"
#include <QDebug>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString qss;
    QFile file(":/macos.qss");
    file.open(QFile::ReadOnly);
    qss=file.readAll();
    file.close();
    a.setStyleSheet(qss);
    MainWindow w;
    w.show();
    return a.exec();
}
