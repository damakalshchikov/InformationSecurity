#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Лабораторная работа №1"));

    MainWindow window;
    window.show();

    return app.exec();
}
