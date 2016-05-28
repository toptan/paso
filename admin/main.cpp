#include <QApplication>
#include "mainwindow.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    qRegisterMetaType<paso::comm::LoginResponse>("LoginResponse");
    paso::admin::MainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
