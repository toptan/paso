#include <QCoreApplication>

#include "pasoserver.h"

using namespace paso::server;

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    PasoServer server(&app);
    server.loadConfiguration();
    server.initDatabaseSystem();
    server.startServer();
    return app.exec();
}
