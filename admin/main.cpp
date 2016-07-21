#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QLibraryInfo>
#include <QStyleFactory>
#include <QTranslator>

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    qRegisterMetaType<paso::comm::LoginResponse>("LoginResponse");
    QTranslator translator;
    translator.load(
        QLocale::Serbian,
        "/Users/toptan/Documents/Work/diplomski/git/paso/build/Debug/paso_sr_RS.qm");
    app.installTranslator(&translator);
    paso::admin::MainWindow mainWindow;
    app.setStyleSheet("QLineEdit[error='true'] { color: #FF0000 }");
    mainWindow.show();
    return app.exec();
}
