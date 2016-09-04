#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QLibraryInfo>
#include <QLocale>
#include <QStyleFactory>
#include <QSysInfo>
#include <QTranslator>

int main(int argc, char **argv) {
    QLocale::setDefault(QLocale(QLocale::Serbian, QLocale::Serbia));
    QApplication app(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    qRegisterMetaType<paso::comm::LoginResponse>("LoginResponse");
    qRegisterMetaType<QList<QPersistentModelIndex>>();
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>();
    QTranslator translator;
    if (QSysInfo::kernelType() == "linux") {
        translator.load(QLocale::Serbian,
                        "/home/toptan/Documents/Work/diplomski/"
                        "git/paso/build/Debug/paso_sr_RS.qm");
    } else if (QSysInfo::kernelType() == "darwin") {
        translator.load(QLocale::Serbian,
                        "/Users/toptan/Documents/Work/diplomski/"
                        "git/paso/build/Debug/paso_sr_RS.qm");
    }
    app.installTranslator(&translator);
    paso::admin::MainWindow mainWindow;
    app.setStyleSheet("QLineEdit[error='true'] { color: #FF0000 }");
    mainWindow.show();
    return app.exec();
}
