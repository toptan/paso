#include "testmainwindow.h"

#include "activityform.h"
#include "commdata.h"
#include "courseform.h"
#include "data.h"
#include "listform.h"
#include "mainwindow.h"
#include "roomform.h"
#include "studentform.h"
#include "teacherform.h"
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
    QCOMPARE(stackedWidget->count(), 7);
    auto courseForm = stackedWidget->findChild<CourseForm *>();
    auto roomForm = stackedWidget->findChild<RoomForm *>();
    auto studentForm = stackedWidget->findChild<StudentForm *>();
    auto teacherForm = stackedWidget->findChild<TeacherForm *>();
    auto systemUsersForm = stackedWidget->findChild<SystemUsersForm *>();
    auto listForm = stackedWidget->findChild<ListForm *>();
    auto activityForm = stackedWidget->findChild<ActivityForm *>();

    QVERIFY(courseForm != nullptr);
    QVERIFY(roomForm != nullptr);
    QVERIFY(studentForm != nullptr);
    QVERIFY(teacherForm != nullptr);
    QVERIFY(systemUsersForm != nullptr);
    QVERIFY(listForm != nullptr);
    QVERIFY(activityForm != nullptr);
    window->hide();
    QApplication::processEvents();
    delete window;

    window = new MainWindow();
    user.setRole(SystemRole::ADMINISTRATOR);
    response =
        make_shared<LoginResponse>(user, "QSQLITE", ":memory:", "", "", "", 0);
    window->loginFinished(*response);
    stackedWidget = window->findChild<QStackedWidget *>();
    QCOMPARE(stackedWidget->count(), 2);
    systemUsersForm = stackedWidget->findChild<SystemUsersForm *>();
    QVERIFY(systemUsersForm != nullptr);
    roomForm = stackedWidget->findChild<RoomForm *>();
    QVERIFY(roomForm != nullptr);
    window->hide();
    QApplication::processEvents();
    delete window;

    window = new MainWindow();
    user.setRole(SystemRole::MANAGER);
    response =
        make_shared<LoginResponse>(user, "QSQLITE", ":memory:", "", "", "", 0);
    window->loginFinished(*response);
    stackedWidget = window->findChild<QStackedWidget *>();
    QCOMPARE(stackedWidget->count(), 5);
    courseForm = stackedWidget->findChild<CourseForm *>();
    studentForm = stackedWidget->findChild<StudentForm *>();
    listForm = stackedWidget->findChild<ListForm *>();
    activityForm = stackedWidget->findChild<ActivityForm *>();
    teacherForm = stackedWidget->findChild<TeacherForm *>();
    QVERIFY(roomForm != nullptr);
    QVERIFY(studentForm != nullptr);
    QVERIFY(listForm != nullptr);
    QVERIFY(activityForm != nullptr);
    QVERIFY(teacherForm != nullptr);
    window->hide();
    QApplication::processEvents();
    delete window;

    window = new MainWindow();
    user.setRole(SystemRole::INVALID_ROLE);
    response =
        make_shared<LoginResponse>(user, "QSQLITE", ":memory:", "", "", "", 0);
    window->loginFinished(*response);
    stackedWidget = window->findChild<QStackedWidget *>();
    QCOMPARE(stackedWidget->count(), 0);
    window->hide();
    QApplication::processEvents();
    delete window;
}
