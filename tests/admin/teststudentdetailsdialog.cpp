#include "teststudentdetailsdialog.h"

#include "pasodb.h"
#include "studentdetailsdialog.h"

#include <QDebug>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlDatabase>
#include <QSqlError>
#include <QTableView>
#include <QTimer>

using namespace std;
using namespace paso::data::entity;
using namespace paso::db;
using namespace paso::admin;

TestStudentDetailsDialog::TestStudentDetailsDialog() : TestBase() {}

void TestStudentDetailsDialog::testDataRefresh() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT, EXPIRY_DATE)"
            "          VALUES('L1', false, false, '1977-01-05')");
    auto listId = manager.getList("L1", error)->id();
    manager.importStudent("2001/2001,Student,1,,3", error);
    manager.importStudent("2002/2002,Student,2,,3", error);
    manager.importStudent("2003/2003,Student,3,,3", error);
    manager.importStudent("2004/2004,Student,4,,3", error);
    auto student = manager.getStudentByIndexNumber("2001/2001", error);
    StudentDetailsDialog dialog(*student);
    dialog.show();
    QTest::qWaitForWindowExposed(&dialog);
    auto coursesTableView = dialog.findChild<QTableView *>("coursesTableView");
    auto listsTableView = dialog.findChild<QTableView *>("listsTableView");
    QCOMPARE(coursesTableView->model()->rowCount(), 0);
    QCOMPARE(listsTableView->model()->rowCount(), 1);

    manager.enlistStudentsToCourse("IR3SP", {"2001/2001", "2002/2002"}, error);
    manager.addStudentsToList(listId, {"2001/2001", "2003/2003"}, error);
    auto refreshButton = dialog.findChild<QPushButton *>("REFRESH_BUTTON");
    QTest::mouseClick(refreshButton, Qt::LeftButton);
    QApplication::processEvents();
    QCOMPARE(coursesTableView->model()->rowCount(), 1);
    // Student is implicitly member of the list that is tied to the course.
    // That is the reason why we compare with 3 not 2.
    QCOMPARE(listsTableView->model()->rowCount(), 3);

    db.exec("DROP VIEW LIST_MEMBERS CASCADE");
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
