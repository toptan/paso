#include "testlistdetailsdialog.h"

#include "addremoveentitiesform.h"
#include "listdetailsdialog.h"
#include "pasodb.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <QFileDialog>
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

TestListDetailsDialog::TestListDetailsDialog() : TestBase() {}

void TestListDetailsDialog::testReadOnly() {
    QVariantMap listMap{
        {"ID", 1}, {"NAME", "Lista"}, {"SYSTEM", true}, {"PERMANENT", true}};
    List systemList(listMap);
    listMap["SYSTEM"] = false;
    List permanentList(listMap);
    listMap["PERMANENT"] = false;
    List regularList;

    ListDetailsDialog systemListDialog(systemList);
    systemListDialog.show();
    QTest::qWaitForWindowExposed(&systemListDialog);
    auto addRemoveEntitiesForm =
        systemListDialog.findChild<AddRemoveEntitiesForm *>();
    auto buttonBox = systemListDialog.findChild<QDialogButtonBox *>();
    auto importButton =
        systemListDialog.findChild<QPushButton *>("IMPORT_BUTTON");
    auto saveButton = buttonBox->button(QDialogButtonBox::Save);
    QVERIFY(addRemoveEntitiesForm->readOnly());
    QVERIFY(importButton == nullptr);
    QVERIFY(saveButton == nullptr);

    ListDetailsDialog permanentListDialog(permanentList);
    permanentListDialog.show();
    QTest::qWaitForWindowExposed(&permanentListDialog);
    addRemoveEntitiesForm =
        permanentListDialog.findChild<AddRemoveEntitiesForm *>();
    buttonBox = permanentListDialog.findChild<QDialogButtonBox *>();
    importButton =
        permanentListDialog.findChild<QPushButton *>("IMPORT_BUTTON");
    saveButton = buttonBox->button(QDialogButtonBox::Save);
    QVERIFY(!addRemoveEntitiesForm->readOnly());
    QVERIFY(importButton != nullptr);
    QVERIFY(saveButton != nullptr);

    ListDetailsDialog regularListDialog(regularList);
    regularListDialog.show();
    QTest::qWaitForWindowExposed(&regularListDialog);
    addRemoveEntitiesForm =
        regularListDialog.findChild<AddRemoveEntitiesForm *>();
    buttonBox = regularListDialog.findChild<QDialogButtonBox *>();
    importButton = regularListDialog.findChild<QPushButton *>("IMPORT_BUTTON");
    saveButton = buttonBox->button(QDialogButtonBox::Save);
    QVERIFY(!addRemoveEntitiesForm->readOnly());
    QVERIFY(importButton != nullptr);
    QVERIFY(saveButton != nullptr);
}

void TestListDetailsDialog::testDataLoading() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT) "
            "          VALUES('L1', 'false', 'true')");
    manager.importStudent("2001/2001,Student,1,,3", error);
    manager.importStudent("2002/2002,Student,2,,3", error);
    manager.importStudent("2003/2003,Student,3,,3", error);
    manager.importStudent("2004/2004,Student,4,,3", error);
    auto list = manager.getList("L1", error);
    manager.addStudentsToList(list->id(), {"2001/2001", "2002/2002"}, error);
    ListDetailsDialog dialog(*list);
    dialog.show();
    QTest::qWaitForWindowExposed(&dialog);

    auto form = dialog.findChild<AddRemoveEntitiesForm *>();
    auto sourceTable = form->findChild<QTableView *>("sourceTableView");
    auto destinationTable =
        form->findChild<QTableView *>("destinationTableView");

    auto refreshButton = dialog.findChild<QPushButton *>("REFRESH_BUTTON");
    QCOMPARE(sourceTable->model()->rowCount(), 2);
    QCOMPARE(destinationTable->model()->rowCount(), 2);

    db.exec("DROP VIEW LIST_MEMBERS");
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

void TestListDetailsDialog::testDataRefresh() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT) "
            "          VALUES('L1', 'false', 'true')");
    manager.importStudent("2001/2001,Student,1,,3", error);
    manager.importStudent("2002/2002,Student,2,,3", error);
    manager.importStudent("2003/2003,Student,3,,3", error);
    manager.importStudent("2004/2004,Student,4,,3", error);
    auto list = manager.getList("L1", error);
    manager.addStudentsToList(list->id(), {"2001/2001", "2002/2002"}, error);
    ListDetailsDialog dialog(*list);
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
    auto refreshMessageCallback = [&refreshMessageBoxShown]() {
        auto msgBox =
            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
        QTest::keyClick(msgBox, Qt::Key_Escape);
        refreshMessageBoxShown = true;
    };
    QTimer::singleShot(200, refreshMessageCallback);
    refreshButton->click();
    QApplication::processEvents();
    QVERIFY(refreshMessageBoxShown);
}

void TestListDetailsDialog::testWarningWhenDataIsDirty() {
//    auto db = QSqlDatabase::database(dbName);
//    DBManager manager(dbName);
//    QSqlError error;
//    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT) "
//            "          VALUES('L1', 'false', 'true')");
//    manager.importStudent("2001/2001,Student,1,,3", error);
//    manager.importStudent("2002/2002,Student,2,,3", error);
//    manager.importStudent("2003/2003,Student,3,,3", error);
//    manager.importStudent("2004/2004,Student,4,,3", error);
//    auto list = manager.getList("L1", error);
//    manager.addStudentsToList(list->id(), {"2001/2001", "2002/2002"}, error);
//    ListDetailsDialog dialog(*list);
//    dialog.show();
//    QTest::qWaitForWindowExposed(&dialog);

//    auto form = dialog.findChild<AddRemoveEntitiesForm *>();
//    auto sourceTable = form->findChild<QTableView *>("sourceTableView");
//    auto destinationTable =
//        form->findChild<QTableView *>("destinationTableView");

//    auto buttonBox = dialog.findChild<QDialogButtonBox *>();
//    auto closeButton = buttonBox->button(QDialogButtonBox::Close);
//    auto addButton = form->findChild<QPushButton *>("addButton");
//    auto removeButton = form->findChild<QPushButton *>("removeButton");

//    sourceTable->selectRow(0);
//    QApplication::processEvents();
//    addButton->click();
//    QApplication::processEvents();

//    bool warningShown = false;
//    auto warningYesCallback = [&warningShown]() {
//        auto msgBox =
//            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
//        QTest::keyClick(msgBox, Qt::Key_Return);
//        qDebug() << "YES CALLBACK";
//        warningShown = true;
//    };
//    QTimer::singleShot(200, warningYesCallback);
//    closeButton->click();
//    QApplication::processEvents();
//    QVERIFY(warningShown);
//    QVERIFY(!dialog.isVisible());
//    QCOMPARE(manager.membersOfTheList(list->id(), error).size(), size_t(3));

//    dialog.show();
//    QTest::qWaitForWindowExposed(&dialog);
//    destinationTable->selectRow(0);
//    QApplication::processEvents();
//    removeButton->click();
//    QApplication::processEvents();
//    warningShown = false;
//    auto warningNoCallback = [&warningShown]() {
//        auto msgBox =
//            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
//        auto noButton = msgBox->button(QMessageBox::No);
//        QTest::mouseClick(noButton, Qt::LeftButton);
//        qDebug() << "NO CALLBACK";
//        warningShown = true;
//    };
//    QTimer::singleShot(200, warningNoCallback);
//    closeButton->click();
//    QApplication::processEvents();
//    QVERIFY(warningShown);
//    QVERIFY(!dialog.isVisible());
//    QCOMPARE(manager.membersOfTheList(list->id(), error).size(), size_t(3));
}
