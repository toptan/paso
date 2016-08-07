#include "testmainwindow.h"

#include "commdata.h"
#include "courseform.h"
#include "data.h"
#include "mainwindow.h"
#include "roomform.h"
#include "studentform.h"
#include "systemuser.h"
#include "systemusersform.h"

#include <QAbstractButton>
#include <QJsonObject>
#include <QMessageBox>
#include <QStackedWidget>
#include <QTimer>
#include <memory>

using namespace std;
using namespace paso::admin;
using namespace paso::comm;
using namespace paso::data;
using namespace paso::data::entity;

TestMainWindow::TestMainWindow() : TestBase() {}

void TestMainWindow::testDbConnectionCreation() {
    MainWindow window;
    window.show();
    QTest::qWaitForWindowExposed(&window);

    SystemUser user("root", "", "", "", "", SystemRole::SUPER_USER);
    LoginResponse response(user, "QSQLITE", ":memory:", "", "", "", 0);
    window.createDbConnection("TEST_CONNECTION", response);
    QVERIFY(QSqlDatabase::contains("TEST_CONNECTION"));
    QSqlDatabase::removeDatabase("TEST_CONNECTION");

    LoginResponse invalidResponse(user, "NO_TYPE", ":memory:", "", "", "", 0);
    bool errorShown = false;
    auto errorCallback = [&errorShown]() {
        auto msgBox =
            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
        QTest::mouseClick(msgBox->button(QMessageBox::Ok), Qt::LeftButton);
        errorShown = true;
    };
    QTimer::singleShot(200, errorCallback);
    QApplication::processEvents();
    window.createDbConnection("TEST_CONNECTION", invalidResponse);
    QVERIFY(errorShown);
}

void TestMainWindow::testRolesForms() {
    auto window = new MainWindow();

    SystemUser user("root", "", "", "", "", SystemRole::SUPER_USER);
    auto response =
        make_shared<LoginResponse>(user, "QSQLITE", ":memory:", "", "", "", 0);

    window->loginFinished(*response);
    QApplication::processEvents();
    auto stackedWidget = window->findChild<QStackedWidget *>();
    QCOMPARE(stackedWidget->count(), 4);
    auto courseForm = stackedWidget->findChild<CourseForm *>();
    auto roomForm = stackedWidget->findChild<RoomForm *>();
    auto studentForm = stackedWidget->findChild<StudentForm *>();
    auto systemUsersForm = stackedWidget->findChild<SystemUsersForm *>();
    QVERIFY(courseForm != nullptr);
    QVERIFY(roomForm != nullptr);
    QVERIFY(studentForm != nullptr);
    QVERIFY(systemUsersForm != nullptr);
    delete window;

    window = new MainWindow();
    user.setRole(SystemRole::ADMINISTRATOR);
    response =
        make_shared<LoginResponse>(user, "QSQLITE", ":memory:", "", "", "", 0);
    window->loginFinished(*response);
    stackedWidget = window->findChild<QStackedWidget *>();
    QCOMPARE(stackedWidget->count(), 1);
    systemUsersForm = stackedWidget->findChild<SystemUsersForm *>();
    QVERIFY(systemUsersForm != nullptr);
    delete window;

    window = new MainWindow();
    user.setRole(SystemRole::ROOM_MANAGER);
    response =
        make_shared<LoginResponse>(user, "QSQLITE", ":memory:", "", "", "", 0);
    window->loginFinished(*response);
    stackedWidget = window->findChild<QStackedWidget *>();
    QCOMPARE(stackedWidget->count(), 1);
    roomForm = stackedWidget->findChild<RoomForm *>();
    QVERIFY(roomForm != nullptr);
    delete window;

    window = new MainWindow();
    user.setRole(SystemRole::MANAGER);
    response =
        make_shared<LoginResponse>(user, "QSQLITE", ":memory:", "", "", "", 0);
    window->loginFinished(*response);
    stackedWidget = window->findChild<QStackedWidget *>();
    QCOMPARE(stackedWidget->count(), 2);
    courseForm = stackedWidget->findChild<CourseForm *>();
    studentForm = stackedWidget->findChild<StudentForm *>();
    QVERIFY(roomForm != nullptr);
    QVERIFY(studentForm != nullptr);
    delete window;

    window = new MainWindow();
    user.setRole(SystemRole::INVALID_ROLE);
    response =
        make_shared<LoginResponse>(user, "QSQLITE", ":memory:", "", "", "", 0);
    window->loginFinished(*response);
    stackedWidget = window->findChild<QStackedWidget *>();
    QCOMPARE(stackedWidget->count(), 0);
    delete window;
}
