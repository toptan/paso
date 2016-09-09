#include "testlistdetailsdialog.h"

#include "addremoveentitiesform.h"
#include "listdetailsdialog.h"
#include "logdialog.h"
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

    auto buttonBox = dialog.findChild<QDialogButtonBox *>();
    auto closeButton = buttonBox->button(QDialogButtonBox::Close);
    auto addButton = form->findChild<QPushButton *>("addButton");
    auto removeButton = form->findChild<QPushButton *>("removeButton");

    sourceTable->selectRow(0);
    QApplication::processEvents();
    addButton->click();
    QApplication::processEvents();

    bool warningShown = false;
    auto warningYesCallback = [&warningShown]() {
        auto msgBox =
            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
        QTest::keyClick(msgBox, Qt::Key_Return);
        warningShown = true;
    };
    QTimer::singleShot(200, warningYesCallback);
    closeButton->click();
    QApplication::processEvents();
    QVERIFY(warningShown);
    QVERIFY(!dialog.isVisible());
    QCOMPARE(manager.membersOfTheList(list->id(), error).size(), size_t(3));

    dialog.show();
    QTest::qWaitForWindowExposed(&dialog);
    destinationTable->selectRow(0);
    QApplication::processEvents();
    removeButton->click();
    QApplication::processEvents();
    warningShown = false;
    auto warningNoCallback = [&warningShown]() {
        auto msgBox =
            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
        auto noButton = msgBox->button(QMessageBox::No);
        QTest::mouseClick(noButton, Qt::LeftButton);
        warningShown = true;
    };
    QTimer::singleShot(200, warningNoCallback);
    closeButton->click();
    QApplication::processEvents();
    QVERIFY(warningShown);
    QVERIFY(!dialog.isVisible());
    QCOMPARE(manager.membersOfTheList(list->id(), error).size(), size_t(3));
}

void TestListDetailsDialog::testSavingData() {
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
    auto buttonBox = dialog.findChild<QDialogButtonBox *>();
    auto saveButton = buttonBox->button(QDialogButtonBox::Save);
    auto sourceTable = form->findChild<QTableView *>("sourceTableView");
    auto destinationTable =
        form->findChild<QTableView *>("destinationTableView");

    auto addButton = form->findChild<QPushButton *>("addButton");
    auto removeButton = form->findChild<QPushButton *>("removeButton");
    destinationTable->selectRow(0);
    QApplication::processEvents();
    removeButton->click();
    QApplication::processEvents();
    saveButton->click();
    QApplication::processEvents();
    QVERIFY(dialog.isVisible());
    QCOMPARE(manager.membersOfTheList(list->id(), error).size(), size_t(1));

    db.exec("DROP TABLE MEMBERS CASCADE");
    sourceTable->selectRow(0);
    QApplication::processEvents();
    addButton->click();
    QApplication::processEvents();
    bool messageShown = false;
    auto messageCallback = [&messageShown]() {
        auto msgBox =
            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
        QTest::keyClick(msgBox, Qt::Key_Return);
        messageShown = true;
    };
    QTimer::singleShot(200, messageCallback);
    saveButton->click();
    QApplication::processEvents();
    QVERIFY(messageShown);
    QVERIFY(dialog.isVisible());
}

void TestListDetailsDialog::testImportListStudents() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    db.exec("INSERT INTO LIST(NAME, SYSTEM, PERMANENT) "
            "          VALUES('L1', 'false', 'true')");
    auto list = manager.getList("L1", error);
    const auto listId = list->id();
    manager.importStudent("1996/0164, Tanaskovic,Toplica,toplica@gmail.com,4",
                          error);
    manager.importStudent("2001/2001, Petrovic,petar,pera@gmail.com,2", error);
    manager.importStudent("2000/2000, Jovanovic,jova,jova@gmail.com,3", error);
    manager.addStudentsToList(listId, {"2000/2000"}, error);
    ListDetailsDialog dialog(*list);
    bool importFinished = false;
    auto importFinishedCallback = [&importFinished]() {
        importFinished = true;
    };
    connect(&dialog, &ListDetailsDialog::importDone, importFinishedCallback);
    connect(&dialog, &ListDetailsDialog::importFailed, importFinishedCallback);
    dialog.show();
    QTest::qWaitForWindowExposed(&dialog);

    auto buttonBox = dialog.findChild<QDialogButtonBox *>();
    auto importButton = buttonBox->findChild<QPushButton *>("IMPORT_BUTTON");
    auto refreshButton = buttonBox->findChild<QPushButton *>("REFRESH_BUTTON");
    auto destinationTable =
        dialog.findChild<QTableView *>("destinationTableView");

    QVERIFY(importButton != nullptr);
    QVERIFY(refreshButton != nullptr);
    QVERIFY(destinationTable != nullptr);
    QCOMPARE(destinationTable->model()->rowCount(), 1);

    bool warningShown = false;
    auto warningCallbackNo = [&warningShown]() {
        auto msgBox =
            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
        QTest::keyClick(msgBox, Qt::Key_Escape);
        warningShown = true;
    };
    QTimer::singleShot(200, warningCallbackNo);
    importButton->click();
    QApplication::processEvents();
    QVERIFY(warningShown);

    auto warningCallbackYes = []() {
        auto msgBox =
            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
        auto yesButton = msgBox->button(QMessageBox::Yes);
        QTest::mouseClick(yesButton, Qt::LeftButton);
    };
    QTimer::singleShot(200, warningCallbackYes);
    importButton->click();
    QApplication::processEvents();
    QFileDialog *fileOpenDialog = nullptr;
    int attempt = 0;
    while ((fileOpenDialog = dynamic_cast<QFileDialog *>(
                QApplication::activeModalWidget())) == nullptr &&
           attempt < 10) {
        QTest::qWait(200);
        attempt++;
    }

    // Is file open dialog shown?
    QVERIFY(fileOpenDialog != nullptr);
    QTest::keyClick(fileOpenDialog, Qt::Key_Escape);
    QApplication::processEvents();
    auto importWithNonExistingFileCallback = [&dialog]() {
        dialog.onImportFileSelected("non_existing_file");
    };
    bool errorMessageBoxShown = false;
    auto errorMessageBoxShownCallback = [&errorMessageBoxShown]() {
        auto msgBox =
            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
        QTest::keyClick(msgBox, Qt::Key_Return);
        errorMessageBoxShown = true;
    };
    QTimer::singleShot(0, importWithNonExistingFileCallback);
    QTimer::singleShot(0, errorMessageBoxShownCallback);
    QApplication::processEvents();
    QVERIFY(errorMessageBoxShown);

    LogDialog *logDialog = nullptr;
    dialog.onImportFileSelected(QDir::currentPath() +
                                "/files/members_with_errors.csv");
    attempt = 0;
    while ((logDialog = dynamic_cast<LogDialog *>(
                QApplication::activeModalWidget())) == nullptr &&
           attempt < 10) {
        QTest::qWait(100);
        attempt++;
    }
    QVERIFY(logDialog != nullptr);
    while (!importFinished) {
        QTest::qWait(100);
    }
    buttonBox = logDialog->findChild<QDialogButtonBox *>();
    buttonBox->button(QDialogButtonBox::Close)->click();
    delete logDialog;
    QApplication::processEvents();
    refreshButton->click();
    QApplication::processEvents();
    QCOMPARE(destinationTable->model()->rowCount(), 1);

    logDialog = nullptr;
    attempt = 0;
    importFinished = false;
    dialog.onImportFileSelected(QDir::currentPath() +
                                "/files/list_members.csv");
    while ((logDialog = dynamic_cast<LogDialog *>(
                QApplication::activeModalWidget())) == nullptr &&
           attempt < 10) {
        QTest::qWait(100);
        attempt++;
    }
    QVERIFY(logDialog != nullptr);
    while (!importFinished) {
        QTest::qWait(100);
    }
    buttonBox = logDialog->findChild<QDialogButtonBox *>();
    buttonBox->button(QDialogButtonBox::Close)->click();
    delete logDialog;
    QApplication::processEvents();
    QCOMPARE(destinationTable->model()->rowCount(), 2);

    db.exec("DROP TABLE STUDENT CASCADE");
    logDialog = nullptr;
    attempt = 0;
    importFinished = false;
    dialog.onImportFileSelected(QDir::currentPath() +
                                "/files/list_members.csv");
    while ((logDialog = dynamic_cast<LogDialog *>(
                QApplication::activeModalWidget())) == nullptr &&
           attempt < 10) {
        QTest::qWait(100);
        attempt++;
    }
    QVERIFY(logDialog != nullptr);
    while (!importFinished) {
        QTest::qWait(100);
    }
    buttonBox = logDialog->findChild<QDialogButtonBox *>();
    buttonBox->button(QDialogButtonBox::Close)->click();
    delete logDialog;
    QApplication::processEvents();
    QCOMPARE(destinationTable->model()->rowCount(), 2);

    db.exec("DROP TABLE MEMBER CASCADE");
    logDialog = nullptr;
    attempt = 0;
    importFinished = false;
    dialog.onImportFileSelected(QDir::currentPath() +
                                "/files/list_members.csv");
    while ((logDialog = dynamic_cast<LogDialog *>(
                QApplication::activeModalWidget())) == nullptr &&
           attempt < 10) {
        QTest::qWait(100);
        attempt++;
    }
    QVERIFY(logDialog != nullptr);
    while (!importFinished) {
        QTest::qWait(100);
    }
    buttonBox = logDialog->findChild<QDialogButtonBox *>();
    buttonBox->button(QDialogButtonBox::Close)->click();
    delete logDialog;
    QApplication::processEvents();
    QCOMPARE(destinationTable->model()->rowCount(), 2);
}
