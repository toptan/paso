#include <QApplication>

#include "mainwindow.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    paso::admin::MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
