#include "simulatormainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QLibraryInfo>
#include <QLocale>
#include <QStyleFactory>
#include <QSysInfo>
#include <QTranslator>
#include <QWidget>

int main(int argc, char **argv) {
    QLocale::setDefault(QLocale(QLocale::Serbian, QLocale::Serbia));
    QApplication app(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
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
    SimulatorMainWindow mainWindow;
    mainWindow.show();
    return app.exec();
}
