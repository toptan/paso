#include "testforms.h"

#include "mocks/mockqueryform.h"
#include "mocks/mocktableform.h"

#include <QAction>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QTableView>
#include <QTimer>

TestForms::TestForms() : QObject(), dbName("paso") {}

void TestForms::initTestCase() {
    auto db = QSqlDatabase::addDatabase("QSQLITE", dbName);
    db.setDatabaseName(":memory:");
    db.open();
}

void TestForms::init() {
    auto db = QSqlDatabase::database(dbName);
    QFile in_memory_sql("../in_memory.sql");
    in_memory_sql.open(QIODevice::ReadOnly);
    QTextStream in(&in_memory_sql);
    QString sqlString = in.readAll();
    sqlString.replace("\n", "");
    QStringList commands = sqlString.split(";");
    for (const auto &command : commands) {
        db.exec(command);
    }
    in_memory_sql.close();
}

void TestForms::cleanup() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("DROP VIEW ENLISTED_STUDENTS");
    db.exec("DROP TABLE SYSTEM_USER");
    db.exec("DROP TABLE ENLISTED");
    db.exec("DROP TABLE COURSE");
    db.exec("DROP TABLE ROOM");
    db.exec("DROP TABLE STUDENT");
    db.exec("DROP TABLE PERSON");
}

void TestForms::testTableForm() {
    MockTableForm form;
    QAction *newRecordAction = nullptr;
    QAction *editRecordAction = nullptr;
    QAction *deleteRecordAction = nullptr;
    QAction *refreshAction = nullptr;
    for (auto action : form.toolBarActions()) {
        if (action->objectName() == "NEW_RECORD_ACTION") {
            newRecordAction = action;
        } else if (action->objectName() == "EDIT_RECORD_ACTION") {
            editRecordAction = action;
        } else if (action->objectName() == "DELETE_RECORD_ACTION") {
            deleteRecordAction = action;
        } else if (action->objectName() == "REFRESH_ACTION") {
            refreshAction = action;
        }
    }
    form.show();
    QApplication::processEvents();
    QTest::qWaitForWindowExposed(&form);
    QVERIFY(form.recordEditor() != nullptr);
    auto tableView = form.findChild<QTableView *>();
    QVERIFY(tableView != nullptr);
    QCOMPARE(tableView->model()->columnCount(), 3);
    QCOMPARE(tableView->model()->rowCount(), 1);
    auto codeEdit =
        dynamic_cast<QLineEdit *>(form.recordEditor()->fieldEditors()["CODE"]);
    auto nameEdit =
        dynamic_cast<QLineEdit *>(form.recordEditor()->fieldEditors()["NAME"]);
    auto buttonBox = form.findChild<QDialogButtonBox *>();
    auto saveButton = buttonBox->button(QDialogButtonBox::Save);
    newRecordAction->trigger();
    QApplication::processEvents();
    QVERIFY(!tableView->isEnabled());
    codeEdit->setText("XXXXX");
    nameEdit->setText("YYYYY");
    QTest::mouseClick(saveButton, Qt::LeftButton);
    QApplication::processEvents();
    QCOMPARE(form.model()->rowCount(), 2);
    newRecordAction->trigger();
    QApplication::processEvents();
    codeEdit->setText("XXXXX");
    nameEdit->setText("YYYYY");

    auto timerCallback = []() {
        QMessageBox *msgBox =
            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
        QTest::keyClick(msgBox, Qt::Key_Enter);
    };
    QTimer::singleShot(200, timerCallback);
    QTest::mouseClick(saveButton, Qt::LeftButton);
    QApplication::processEvents();
    QCOMPARE(form.model()->rowCount(), 2);

    tableView->selectRow(0);
    QApplication::processEvents();
    editRecordAction->trigger();
    QApplication::processEvents();
    QVERIFY(!tableView->isEnabled());
    QVERIFY(!codeEdit->text().isEmpty());
    QVERIFY(!nameEdit->text().isEmpty());
    codeEdit->setText("AAAAA");
    nameEdit->setText("BBBBB");
    QTest::mouseClick(saveButton, Qt::LeftButton);
    QApplication::processEvents();
    QCOMPARE(form.model()->lastError().type(), QSqlError::NoError);
    editRecordAction->trigger();
    QApplication::processEvents();
    codeEdit->setText("XXXXX");
    nameEdit->setText("YYYYY");
    QTimer::singleShot(200, timerCallback);
    QTest::mouseClick(saveButton, Qt::LeftButton);
    QApplication::processEvents();
    QCOMPARE(form.model()->record(0).value("CODE").toString(),
             QString("AAAAA"));

    tableView->selectRow(1);
    form.setReadOnly(true);
    QApplication::processEvents();
    deleteRecordAction->trigger();
    QApplication::processEvents();
    QCOMPARE(form.model()->rowCount(), 2);
    tableView->selectRow(1);
    form.setReadOnly(false);
    QApplication::processEvents();
    deleteRecordAction->trigger();
    QApplication::processEvents();
    QCOMPARE(form.model()->rowCount(), 1);
    tableView->clearSelection();
    QApplication::processEvents();
    deleteRecordAction->trigger();
    QApplication::processEvents();
    QCOMPARE(form.model()->rowCount(), 1);

    auto db = QSqlDatabase::database("paso");
    db.exec("INSERT INTO COURSE(CODE, NAME) VALUES('CCCCC', 'DDDDD')");
    refreshAction->trigger();
    QApplication::processEvents();
    QCOMPARE(form.model()->rowCount(), 2);
    tableView->selectRow(1);
    QApplication::processEvents();
    refreshAction->trigger();
    QApplication::processEvents();
    QCOMPARE(tableView->selectionModel()->currentIndex().row(), 1);
    QCOMPARE(form.getSelectedRecord().value("CODE").toString(),
             codeEdit->text());
    QCOMPARE(form.getSelectedRecord().value("NAME").toString(),
             nameEdit->text());
    tableView->clearSelection();
    tableView->selectionModel()->clearSelection();
    tableView->selectionModel()->setCurrentIndex(QModelIndex(),
                                                 QItemSelectionModel::NoUpdate);
    tableView->setCurrentIndex(QModelIndex());
    QApplication::processEvents();
    QVERIFY(form.getSelectedRecord().isEmpty());
    editRecordAction->trigger();
    QApplication::processEvents();

    tableView->selectRow(0);
    QApplication::processEvents();
    db.exec("DROP TABLE COURSE");
    QTimer::singleShot(200, timerCallback);
    deleteRecordAction->trigger();
    QApplication::processEvents();
    QVERIFY(form.model()->lastError().type() != QSqlError::NoError);
}

void TestForms::testQueryForm() {
    MockQueryForm form;
    QAction *refreshAction = nullptr;
    QAction *deleteAction = nullptr;
    for (auto action : form.toolBarActions()) {
        if (action->objectName() == "REFRESH_ACTION") {
            refreshAction = action;
        } else if (action->objectName() == "DELETE_RECORD_ACTION") {
            deleteAction = action;
        }
    }
    form.show();
    QApplication::processEvents();
    QTest::qWaitForWindowExposed(&form);
    QVERIFY(form.recordEditor() != nullptr);
    auto tableView = form.findChild<QTableView *>();
    QVERIFY(tableView != nullptr);
    QSqlError error;
    QCOMPARE(tableView->model()->rowCount(), 1);
    auto db = QSqlDatabase::database("paso");
    db.exec("INSERT INTO COURSE(CODE, NAME) VALUES('CCCCC', 'DDDDD')");
    refreshAction->trigger();
    QApplication::processEvents();
    QCOMPARE(form.model()->rowCount(), 2);
    tableView->selectRow(0);
    deleteAction->trigger();
    QApplication::processEvents();
    QCOMPARE(form.model()->rowCount(), 1);
}

QTEST_MAIN(TestForms)
