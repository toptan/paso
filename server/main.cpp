#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>

#include "pasoserver.h"

#include <syslog.h>

using namespace paso::server;
using namespace std;

void logMessageHandler(QtMsgType type, const QMessageLogContext &context,
                       const QString &message) {
    int logLevel = LOG_NOTICE;
    QString txt;
    switch (type) {
    case QtDebugMsg:
        logLevel = LOG_DEBUG;
        txt = QString("DEBUG   : %1").arg(message);
        break;
    case QtInfoMsg:
        logLevel = LOG_INFO;
        txt = QString("INFO    : %1").arg(message);
        break;
    case QtWarningMsg:
        logLevel = LOG_WARNING;
        txt = QString("WARNING : %1").arg(message);
        break;
    case QtCriticalMsg:
        logLevel = LOG_CRIT;
        txt = QString("CRITICAL: %1").arg(message);
        break;
    case QtFatalMsg:
        logLevel = LOG_ALERT;
        txt = QString("FATAL   : %1").arg(message);
        break;
    }
    syslog(LOG_NOTICE, "%s", txt.toStdString().c_str());
}

int main(int argc, char **argv) {
    setlogmask(LOG_UPTO(LOG_DEBUG));
    openlog("pasoserver", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

    QCoreApplication app(argc, argv);
    qInstallMessageHandler(logMessageHandler);

    QCoreApplication::setApplicationName("pasoserver");
    QCoreApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption configFileOption(
        {"c", "config"}, "Full path to the configuration file.\nDefaults to: "
                         "/etc/pasoserver.conf",
        "config_file", "/etc/pasoserver.conf");
    parser.addOption(configFileOption);
    parser.process(app);

    PasoServer server(&app);

    if (!server.loadConfiguration(parser.value(configFileOption))) {
        return EXIT_FAILURE;
    }
    if (!server.initDatabaseSystem()) {
        return EXIT_FAILURE;
    }
    if (!server.startServer()) {
        return EXIT_FAILURE;
    }

    auto retVal = app.exec();
    qInfo() << "Server exited.";
    closelog();

    return retVal;
}
