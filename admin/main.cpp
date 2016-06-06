#include <QApplication>
#include <QStyleFactory>
#include "mainwindow.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
//    QApplication::setStyle(QStyleFactory::create("Fusion"));
    qRegisterMetaType<paso::comm::LoginResponse>("LoginResponse");
    paso::admin::MainWindow mainWindow;
    app.setStyleSheet("QLineEdit[error='true'] { color: #FF0000 }");
    mainWindow.show();
    return app.exec();
}
