#include <QApplication>
#include <QDebug>
#include <QProcess>
#include <QSqlDatabase>
#include <QStyleFactory>
#include <QtTest>
#include <list>
#include <set>

#include "entity.h"
#include "testactivityadministration.h"
#include "testcommdata.h"
#include "testcommmanager.h"
#include "testcourseadministration.h"
#include "testcoursedetailsdialog.h"
#include "testdata.h"
#include "testforms.h"
#include "testlistadministration.h"
#include "testlistdetailsdialog.h"
#include "testlogindialog.h"
#include "testmainwindow.h"
#include "testmodels.h"
#include "testpasodb.h"
#include "testroomadministration.h"
#include "teststudentadministration.h"
#include "teststudentdetailsdialog.h"
#include "testsystemuseradministration.h"
#include "testwidgets.h"

using namespace std;

QT_BEGIN_NAMESPACE
QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS
QT_END_NAMESPACE

void createDatabase() {
    QProcess createProcess;
    QStringList arguments;
    arguments << "-h"
              << "127.0.0.1"
              << "-U"
              << "pasotest"
              << "-f"
              << "create_db.sql"
              << "postgres";
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PGPASSWORD", "pasotest");
    createProcess.setProcessEnvironment(env);
    createProcess.setWorkingDirectory(QApplication::applicationDirPath());
    createProcess.start("psql", arguments);
    if (!createProcess.waitForStarted()) {
        qCritical() << "Failed to start test database creation.";
        qCritical() << QString(createProcess.readAllStandardError());
        qCritical() << QString(createProcess.readAllStandardOutput());
        qCritical() << createProcess.workingDirectory();
        qCritical() << createProcess.errorString();
        exit(-1);
    }
    if (!createProcess.waitForFinished()) {
        qCritical() << "Failed to finish test database creation.";
        qCritical() << QString(createProcess.readAllStandardError());
        qCritical() << QString(createProcess.readAllStandardOutput());
        exit(-1);
    }
    if (createProcess.exitCode() != 0) {
        qCritical() << "Failed to create test database.";
        qCritical() << QString(createProcess.readAllStandardError());
        qCritical() << QString(createProcess.readAllStandardOutput());
        exit(createProcess.exitCode());
    }

    auto db = QSqlDatabase::addDatabase("QPSQL", "paso");
    db.setDatabaseName("pasotest");
    db.setHostName("127.0.0.1");
    db.setPassword("pasotest");
    db.setUserName("pasotest");
    db.open();
}

void dropDatabase() {
    QProcess dropProcess;
    QStringList arguments;
    arguments << "-h"
              << "127.0.0.1"
              << "-U"
              << "pasotest"
              << "-f"
              << "drop_db.sql"
              << "postgres";
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PGPASSWORD", "pasotest");
    dropProcess.setProcessEnvironment(env);
    dropProcess.setWorkingDirectory(QApplication::applicationDirPath());
    dropProcess.start("psql", arguments);
    if (!dropProcess.waitForStarted()) {
        qCritical() << "Failed to start test database drop.";
        qCritical() << dropProcess.errorString();
        exit(-1);
    }
    if (!dropProcess.waitForFinished()) {
        qCritical() << "Failed to finish test database drop.";
        exit(-1);
    }
    if (dropProcess.exitCode() != 0) {
        qCritical() << "Failed to drop test database.";
        qCritical() << QString(dropProcess.readAllStandardError());
        qCritical() << QString(dropProcess.readAllStandardOutput());
        exit(dropProcess.exitCode());
    }
}

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    app.setAttribute(Qt::AA_Use96Dpi, true);

    QTEST_DISABLE_KEYPAD_NAVIGATION
    QTEST_ADD_GPU_BLACKLIST_SUPPORT
    QTEST_SET_MAIN_SOURCE_PATH
    qRegisterMetaType<paso::comm::LoginResponse>("LoginResponse");
    qRegisterMetaType<QList<QPersistentModelIndex>>();
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>();
    list<QObject *> testCases;
    createDatabase();

    int status = 0;
    auto ASSERT_TEST = [&status, argc, argv](QObject *obj) {
        status |= QTest::qExec(obj, argc, argv);
        delete obj;
    };

    testCases.push_back(new TestActivityAdministration);
    testCases.push_back(new TestLoginDialog);
    testCases.push_back(new TestCommData);
    testCases.push_back(new TestCommManager);
    testCases.push_back(new TestData);
    testCases.push_back(new TestModels);
    testCases.push_back(new TestPasoDB);
    testCases.push_back(new TestForms);
    testCases.push_back(new TestWidgets);
    testCases.push_back(new TestCourseAdministration);
    testCases.push_back(new TestListAdministration);
    testCases.push_back(new TestRoomAdministration);
    testCases.push_back(new TestSystemUserAdministration);
    testCases.push_back(new TestStudentAdministration);
    testCases.push_back(new TestCourseDetailsDialog);
    testCases.push_back(new TestListDetailsDialog);
    testCases.push_back(new TestStudentDetailsDialog);
    testCases.push_back(new TestMainWindow);

    for (auto testCase : testCases) {
        ASSERT_TEST(testCase);
    }
    dropDatabase();
    TestBase::printTestStats();
    return status;
}
