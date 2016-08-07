#include "testcoursedetailsdialog.h"

#include "addremoveentitiesform.h"
#include "coursedetailsdialog.h"
#include "pasodb.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlError>
#include <QTableView>
#include <QtConcurrent>

using namespace std;
using namespace paso::data::entity;
using namespace paso::db;
using namespace paso::admin;
using namespace paso::widget;

TestCourseDetailsDialog::TestCourseDetailsDialog() : TestBase() {}

void TestCourseDetailsDialog::testDataLoading() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    manager.importStudent("2001/2001,Student,1,,3", error);
    manager.importStudent("2002/2002,Student,2,,3", error);
    manager.importStudent("2003/2003,Student,3,,3", error);
    manager.importStudent("2004/2004,Student,4,,3", error);
    manager.enlistStudentsToCourse("IR3SP", {"2001/2001", "2002/2002"}, error);
    auto course = manager.getCourse("IR3SP", error);
    CourseDetailsDialog dialog(*course);
    dialog.show();
    QTest::qWaitForWindowExposed(&dialog);

    auto form = dialog.findChild<AddRemoveEntitiesForm *>();
    auto sourceTable = form->findChild<QTableView *>("sourceTableView");
    auto destinationTable =
        form->findChild<QTableView *>("destinationTableView");

    auto refreshButton = dialog.findChild<QPushButton *>("REFRESH_BUTTON");
    QCOMPARE(sourceTable->model()->rowCount(), 2);
    QCOMPARE(destinationTable->model()->rowCount(), 2);

    db.exec("DROP VIEW ENLISTED_STUDENTS");
    bool errorMessageBoxShown = false;
    auto errorMessageCallback = [&errorMessageBoxShown]() {
        auto msgBox =
            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
        QTest::keyClick(msgBox, Qt::Key_Return);
        errorMessageBoxShown = true;
    };
    QTimer::singleShot(200, errorMessageCallback);
    refreshButton->click();
    QApplication::processEvents();
    QVERIFY(errorMessageBoxShown);
}

void TestCourseDetailsDialog::testDataRefresh() {
    DBManager manager(dbName);
    QSqlError error;
    manager.importStudent("2001/2001,Student,1,,3", error);
    manager.importStudent("2002/2002,Student,2,,3", error);
    manager.importStudent("2003/2003,Student,3,,3", error);
    manager.importStudent("2004/2004,Student,4,,3", error);
    manager.enlistStudentsToCourse("IR3SP", {"2001/2001", "2002/2002"}, error);
    auto course = manager.getCourse("IR3SP", error);
    CourseDetailsDialog dialog(*course);
    dialog.show();
    QTest::qWaitForWindowExposed(&dialog);

    auto form = dialog.findChild<AddRemoveEntitiesForm *>();
    auto sourceTable = form->findChild<QTableView *>("sourceTableView");
    auto destinationTable =
        form->findChild<QTableView *>("destinationTableView");

    auto addButton = form->findChild<QPushButton *>("addButton");
    auto removeButton = form->findChild<QPushButton *>("removeButton");
    auto refreshButton = dialog.findChild<QPushButton *>("REFRESH_BUTTON");
    sourceTable->selectRow(0);
    QApplication::processEvents();
    addButton->clicked();
    destinationTable->selectRow(0);
    removeButton->click();

    bool refreshMessageBoxShown = false;
    auto refreshMessageCallback = [&refreshMessageBoxShown] () {
        auto msgBox = dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
        QTest::keyClick(msgBox, Qt::Key_Escape);
        refreshMessageBoxShown = true;
    };
    QTimer::singleShot(200, refreshMessageCallback);
    refreshButton->click();
    QApplication::processEvents();
    QVERIFY(refreshMessageBoxShown);

}
