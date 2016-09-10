#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>

#include "pasoserver.h"

#include <unistd.h>
#include <csignal>
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

void signalHandler(int) {
    qInfo() << "Shutting down PaSo server.";
    QCoreApplication::instance()->quit();
}

int main(int argc, char **argv) {
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);
    setlogmask(LOG_UPTO(LOG_DEBUG));
    openlog("pasoserver", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

    QCoreApplication app(argc, argv);
    qInstallMessageHandler(logMessageHandler);

    QCoreApplication::setApplicationName("pasoserver");
    QCoreApplication::setApplicationVersion(VERSION);

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption configFileOption(
        {"c", "config"}, "Full path to the configuration file.\nDefaults to: "
                         "/etc/pasoserver.conf",
        "config_file", "/etc/pasoserver.conf");

    const QCommandLineOption daemonizeOption(
    {"d", "daemonize"}, "If given the process will fork in background and become deamon.");
    parser.addOption(configFileOption);
    parser.addOption(daemonizeOption);
    parser.process(app);

    if (parser.isSet(daemonizeOption)) {
        qInfo() << "Forking in background.";
        pid_t pid, sid;
        // Fork the parrent process.
        pid = fork();

        if (pid < 0) {
            qCritical() << "Failed to daemonize PaSo server.";
            return EXIT_FAILURE;
        }

        if (pid > 0) {
            // We are parent process and we should exit now.
            return 0;
        }

        // Create unique signature for child process.
        sid = setsid();

        if (sid < 0) {
            qCritical() << "Failed to create unique Signature Id for PaSo server.";
            return EXIT_FAILURE;
        }

        // Close stdin, stdout and stderr
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }

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
    qInfo() << "Server shut down.";
    closelog();

    return retVal;
}
