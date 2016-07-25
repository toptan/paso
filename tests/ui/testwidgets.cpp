#include "testwidgets.h"

#include "addremoveentitiesform.h"
#include "course.h"
#include "entity.h"
#include "recordeditorwidget.h"

#include "mocks/mockalwaysinvalidrecordvalidator.h"
#include "mocks/mockalwaysvalidrecordvalidator.h"
#include "mocks/mockrecordeditorwidget.h"
#include "mocks/mocktableform.h"

#include <QAbstractTableModel>
#include <QDebug>
#include <QPushButton>
#include <QSignalSpy>
#include <QSortFilterProxyModel>
#include <QSqlError>
#include <QSqlField>
#include <QSqlRecord>
#include <QTableView>
#include <QtConcurrent>
#include <memory>

using namespace std;
using namespace paso::data::entity;
using namespace paso::widget;

TestWidgets::TestWidgets() : QObject(), dbName("paso") {}

void TestWidgets::initTestCase() {
    auto db = QSqlDatabase::addDatabase("QSQLITE", dbName);
    db.setDatabaseName(":memory:");
    db.open();
}

void TestWidgets::init() {
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

void TestWidgets::cleanup() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("DROP VIEW ENLISTED_STUDENTS");
    db.exec("DROP TABLE SYSTEM_USER");
    db.exec("DROP TABLE ENLISTED");
    db.exec("DROP TABLE COURSE");
    db.exec("DROP TABLE ROOM");
    db.exec("DROP TABLE STUDENT");
    db.exec("DROP TABLE PERSON");
}

void TestWidgets::testAddRemoveEntityWidget() {
    AddRemoveEntitiesForm form;
    QStringList columns{"NAME", "CODE"};
    QMap<QString, QString> columnNames{{"NAME", "Predmet"}, {"CODE", "Šifra"}};

    EntityVector sourceData{
        make_shared<Course>("IR3SP", "Sistemsko programiranje", 3),
        make_shared<Course>("IR3BP1", "Baze podataka 1", 5)};
    EntityVector destinationData;

    form.setData("Source", columns, columnNames, sourceData, "Destination",
                 columns, columnNames, destinationData);
    auto sourceTable = form.findChild<QTableView *>("sourceTableView");
    auto destinationTable =
        form.findChild<QTableView *>("destinationTableView");

    auto addButton = form.findChild<QPushButton *>("addButton");
    auto removeButton = form.findChild<QPushButton *>("removeButton");
    auto resetButton = form.findChild<QPushButton *>("resetButton");

    sourceTable->selectRow(0);
    QTest::mouseClick(addButton, Qt::LeftButton);
    QCOMPARE(form.addedEntities().size(), size_t(1));
    QVERIFY(form.removedEntities().empty());
    QVERIFY(form.dirty());

    sourceTable->clearSelection();
    QTest::mouseClick(addButton, Qt::LeftButton);
    QCOMPARE(form.addedEntities().size(), size_t(1));
    QVERIFY(form.removedEntities().empty());
    QVERIFY(form.dirty());

    sourceTable->selectRow(0);
    QTest::mouseClick(addButton, Qt::LeftButton);
    QCOMPARE(form.addedEntities().size(), size_t(2));
    QVERIFY(form.removedEntities().empty());
    QVERIFY(form.dirty());

    destinationTable->clearSelection();
    QTest::mouseClick(removeButton, Qt::LeftButton);
    QCOMPARE(form.addedEntities().size(), size_t(2));
    QVERIFY(form.removedEntities().empty());
    QVERIFY(form.dirty());

    destinationTable->selectRow(0);
    QTest::mouseClick(removeButton, Qt::LeftButton);
    QCOMPARE(form.addedEntities().size(), size_t(1));
    QCOMPARE(form.removedEntities().size(), size_t(1));
    QVERIFY(form.dirty());

    QTest::mouseClick(resetButton, Qt::LeftButton);
    QVERIFY(form.addedEntities().empty());
    QVERIFY(form.removedEntities().empty());
    QVERIFY(!form.dirty());

    auto sourceRowCount =
        dynamic_cast<QSortFilterProxyModel *>(sourceTable->model())
            ->sourceModel()
            ->rowCount();
    for (int row = 0; row < sourceRowCount; row++) {
        sourceTable->selectRow(row);
    }
    QTest::mouseClick(addButton, Qt::LeftButton);
    QCOMPARE(form.addedEntities().size(), size_t(2));
    QVERIFY(form.removedEntities().empty());
    QVERIFY(form.dirty());

    auto destinationRowCount =
        dynamic_cast<QSortFilterProxyModel *>(destinationTable->model())
            ->sourceModel()
            ->rowCount();
    for (int row = 0; row < destinationRowCount; row++) {
        destinationTable->selectRow(row);
    }
    QTest::mouseClick(removeButton, Qt::LeftButton);
    QVERIFY(form.addedEntities().empty());
    QCOMPARE(form.removedEntities().size(), size_t(2));
    QVERIFY(form.dirty());
}

void TestWidgets::testRecordValidator() {
    MockAlwaysValidRecordValidator validator;
    QVERIFY(validator.validate(QSqlRecord()) == nullptr);

    QWidget *editor = new QWidget();
    auto timerCallback = [&editor]() {
        QMessageBox *msgBox =
            editor->findChild<QMessageBox *>("", Qt::FindDirectChildrenOnly);
        QVERIFY(editor->property("error").toBool());
        QString title = "AAA";
        if (QSysInfo::kernelType() == "darwin") {
            // Required by OS X HIG.
            title = "";
        }
        QCOMPARE(msgBox->windowTitle(), title);
        QCOMPARE(msgBox->text(), QString("BBB"));
        QCOMPARE(msgBox->detailedText(), QString("CCC"));
        QTest::keyClick(msgBox, Qt::Key_Enter);
    };

    bool msgBoxFinished = false;
    auto work = [&editor, &msgBoxFinished, timerCallback]() {
        QTimer::singleShot(200, timerCallback);
        showEntryError(editor, "AAA", "BBB", "CCC");
        QVERIFY(!editor->property("error").toBool());
        delete editor;
        editor = nullptr;
        msgBoxFinished = true;
    };
    work();
    QVERIFY(editor == nullptr);
}

void TestWidgets::testRecordEditorWidget() {
    FieldTypes fieldTypes{{"LINE_EDIT", FieldType::LineEdit},
                          {"MASKED_LINE_EDIT", FieldType::MaskedLineEdit},
                          {"PASSWORD_EDIT", FieldType::PasswordEdit},
                          {"NUMBER_EDIT", FieldType::NumberEdit},
                          {"COMBO_BOX", FieldType::ComboBox}};
    QSqlRecord record;
    record.append(QSqlField("LINE_EDIT", QVariant::String));
    record.append(QSqlField("MASKED_LINE_EDIT", QVariant::String));
    record.append(QSqlField("PASSWORD_EDIT", QVariant::String));
    record.append(QSqlField("ID", QVariant::LongLong));
    record.append(QSqlField("NUMBER_EDIT", QVariant::Int));
    record.append(QSqlField("COMBO_BOX", QVariant::Int));
    QVariantMap columnLabels{{"LINE_EDIT", "LineEdit"},
                             {"MASKED_LINE_EDIT", "MaskedLineEdit"},
                             {"PASSWORD_EDIT", "PasswordEdit"},
                             {"NUMBER_EDIT", "NumberEdit"},
                             {"COMBO_BOX", "ComboBox"}};
    MockAlwaysValidRecordValidator alwaysValidValidator;
    MockAlwaysInvalidRecordValidator alwaysInvalidValidator;
    MockRecordEditorWidget w(fieldTypes);
    w.setupUi(columnLabels, record);
    w.setValidator(&alwaysValidValidator);
    w.show();
    // Form layout, button box + 2 x (columns - ID column)
    QCOMPARE(w.children().size(), 12);
    QCOMPARE(w.fieldTypes(), fieldTypes);
    QCOMPARE(w.fieldEditors().keys(), fieldTypes.keys());
    record.setValue("ID", 42);
    record.setValue("LINE_EDIT", "Line Edit");
    record.setValue("MASKED_LINE_EDIT", "Masked/Line/Edit");
    record.setValue("PASSWORD_EDIT", "Password Edit");
    record.setValue("NUMBER_EDIT", 9);
    record.setValue("COMBO_BOX", 2);

    QVariantList stringValues{"Line Edit", "Masked/Line/Edit", "Password Edit"};
    w.onDisplayRecord(record);
    QLineEdit *lineEdit = nullptr;
    QLineEdit *maskedLineEdit = nullptr;
    QLineEdit *passwordEdit = nullptr;
    for (const auto child : w.findChildren<QLineEdit *>()) {
        if (child->text() == "Line Edit") {
            stringValues.removeOne("Line Edit");
            QVERIFY(child->isReadOnly());
            lineEdit = child;
        }
        if (child->text() == "Masked/Line/Edit") {
            stringValues.removeOne("Masked/Line/Edit");
            QCOMPARE(child->inputMask(), QString("AAAAAA/AAAA/AAAA"));
            QVERIFY(child->isReadOnly());
            maskedLineEdit = child;
        }
        if (child->text() == "Password Edit") {
            stringValues.removeOne("Password Edit");
            QCOMPARE(child->echoMode(), QLineEdit::Password);
            QVERIFY(child->isReadOnly());
            passwordEdit = child;
        }
    }
    QVERIFY(stringValues.empty());
    auto spinBox = w.findChild<QSpinBox *>();
    QCOMPARE(spinBox->value(), 9);
    QCOMPARE(spinBox->minimum(), 6);
    QCOMPARE(spinBox->maximum(), 12);
    QVERIFY(!spinBox->isEnabled());
    auto comboBox = w.findChild<QComboBox *>();
    QCOMPARE(comboBox->model()->rowCount(), 3);
    QCOMPARE(comboBox->currentIndex(), 1);
    QCOMPARE(comboBox->currentText(), QString("Dva"));
    QCOMPARE(comboBox->currentData(), QVariant(2));
    QVERIFY(!comboBox->isEnabled());
    auto buttonBox = w.findChild<QDialogButtonBox *>();
    QVERIFY(!buttonBox->isVisible());
    QCOMPARE(w.fieldEditors().value("LINE_EDIT"), lineEdit);
    QCOMPARE(w.fieldEditors().value("MASKED_LINE_EDIT"), maskedLineEdit);
    QCOMPARE(w.fieldEditors().value("PASSWORD_EDIT"), passwordEdit);
    QCOMPARE(w.fieldEditors().value("NUMBER_EDIT"), spinBox);
    QCOMPARE(w.fieldEditors().value("COMBO_BOX"), comboBox);
    auto saveButton = buttonBox->button(QDialogButtonBox::Save);
    auto cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    QApplication::processEvents();

    w.clearData();
    QVERIFY(lineEdit->text().isEmpty());
    QCOMPARE(maskedLineEdit->text(), QString("//"));
    QVERIFY(passwordEdit->text().isEmpty());
    QCOMPARE(spinBox->value(), spinBox->minimum());
    QVERIFY(spinBox->text().isEmpty());
    QCOMPARE(comboBox->currentIndex(), -1);

    w.clearData();
    w.onEditExistingRecord(record);
    QApplication::processEvents();
    QCOMPARE(lineEdit->text(), QString("Line Edit"));
    QVERIFY(!lineEdit->isReadOnly());
    QCOMPARE(maskedLineEdit->text(), QString("Masked/Line/Edit"));
    QVERIFY(!lineEdit->isReadOnly());
    QCOMPARE(passwordEdit->text(), QString("Password Edit"));
    QVERIFY(!passwordEdit->isReadOnly());
    QCOMPARE(spinBox->value(), 9);
    QVERIFY(spinBox->isEnabled());
    QCOMPARE(comboBox->currentIndex(), 1);
    QCOMPARE(comboBox->currentText(), QString("Dva"));
    QCOMPARE(comboBox->currentData(), QVariant(2));
    QVERIFY(comboBox->isEnabled());
    QVERIFY(buttonBox->isVisible());

    QTest::mouseClick(saveButton, Qt::LeftButton);
    QApplication::processEvents();
    w.saveSuccessfull();
    QCOMPARE(lineEdit->text(), QString("Line Edit"));
    QVERIFY(lineEdit->isReadOnly());
    QCOMPARE(maskedLineEdit->text(), QString("Masked/Line/Edit"));
    QVERIFY(lineEdit->isReadOnly());
    QCOMPARE(passwordEdit->text(), QString("Password Edit"));
    QVERIFY(passwordEdit->isReadOnly());
    QCOMPARE(spinBox->value(), 9);
    QVERIFY(!spinBox->isEnabled());
    QCOMPARE(comboBox->currentIndex(), 1);
    QCOMPARE(comboBox->currentText(), QString("Dva"));
    QCOMPARE(comboBox->currentData(), QVariant(2));
    QVERIFY(!comboBox->isEnabled());
    QVERIFY(!buttonBox->isVisible());

    w.onEditExistingRecord(record);
    QApplication::processEvents();
    lineEdit->setText("FOO");
    maskedLineEdit->setText("BBBBBB/CCCC/DDDD");
    passwordEdit->setText("BAR");
    spinBox->setValue(10);
    comboBox->setCurrentIndex(2);
    QTest::mouseClick(cancelButton, Qt::LeftButton);
    QApplication::processEvents();
    QCOMPARE(lineEdit->text(), QString("Line Edit"));
    QVERIFY(lineEdit->isReadOnly());
    QCOMPARE(maskedLineEdit->text(), QString("Masked/Line/Edit"));
    QVERIFY(lineEdit->isReadOnly());
    QCOMPARE(passwordEdit->text(), QString("Password Edit"));
    QVERIFY(passwordEdit->isReadOnly());
    QCOMPARE(spinBox->value(), 9);
    QVERIFY(!spinBox->isEnabled());
    QCOMPARE(comboBox->currentIndex(), 1);
    QCOMPARE(comboBox->currentText(), QString("Dva"));
    QCOMPARE(comboBox->currentData(), QVariant(2));
    QVERIFY(!comboBox->isEnabled());
    QVERIFY(!buttonBox->isVisible());

    w.clearData();
    record.clearValues();
    w.onEditNewRecord(record);
    QApplication::processEvents();
    QVERIFY(lineEdit->text().isEmpty());
    QVERIFY(!lineEdit->isReadOnly());
    QCOMPARE(maskedLineEdit->text(), QString("//"));
    QVERIFY(!lineEdit->isReadOnly());
    QVERIFY(passwordEdit->text().isEmpty());
    QVERIFY(!passwordEdit->isReadOnly());
    QCOMPARE(spinBox->value(), 6);
    QVERIFY(spinBox->isEnabled());
    QCOMPARE(comboBox->currentIndex(), 0);
    QCOMPARE(comboBox->currentText(), QString("Jedan"));
    QCOMPARE(comboBox->currentData(), QVariant(1));
    QVERIFY(comboBox->isEnabled());
    QVERIFY(buttonBox->isVisible());
    QVERIFY(lineEdit->hasFocus());

    lineEdit->setText("FOO");
    maskedLineEdit->setText("BBBBBB/CCCC/DDDD");
    passwordEdit->setText("BAR");
    spinBox->setValue(10);
    comboBox->setCurrentIndex(2);
    QTest::mouseClick(cancelButton, Qt::LeftButton);
    QApplication::processEvents();
    QVERIFY(lineEdit->text().isEmpty());
    QVERIFY(lineEdit->isReadOnly());
    QCOMPARE(maskedLineEdit->text(), QString("//"));
    QVERIFY(lineEdit->isReadOnly());
    QVERIFY(passwordEdit->text().isEmpty());
    QVERIFY(passwordEdit->isReadOnly());
    QCOMPARE(spinBox->value(), 6);
    QVERIFY(!spinBox->isEnabled());
    QCOMPARE(comboBox->currentIndex(), -1);
    QVERIFY(!comboBox->isEnabled());
    QVERIFY(!buttonBox->isVisible());

    lineEdit->setText("FOO");
    maskedLineEdit->setText("BBBBBB/CCCC/DDDD");
    passwordEdit->setText("BAR");
    spinBox->setValue(10);
    comboBox->setCurrentIndex(2);
    QTest::mouseClick(saveButton, Qt::LeftButton);
    QApplication::processEvents();
    w.saveSuccessfull();
    QCOMPARE(lineEdit->text(), QString("FOO"));
    QVERIFY(lineEdit->isReadOnly());
    QCOMPARE(maskedLineEdit->text(), QString("BBBBBB/CCCC/DDDD"));
    QVERIFY(lineEdit->isReadOnly());
    QCOMPARE(passwordEdit->text(), QString("BAR"));
    QVERIFY(passwordEdit->isReadOnly());
    QCOMPARE(spinBox->value(), 10);
    QVERIFY(!spinBox->isEnabled());
    QCOMPARE(comboBox->currentIndex(), 2);
    QVERIFY(!comboBox->isEnabled());
    QVERIFY(!buttonBox->isVisible());

    // Focus checks
    w.writableField = "MASKED_LINE_EDIT";
    w.onEditExistingRecord(record);
    QApplication::processEvents();
    QVERIFY(maskedLineEdit->hasFocus());
    QTest::mouseClick(cancelButton, Qt::LeftButton);
    QApplication::processEvents();
    w.writableField = "PASSWORD_EDIT";
    w.onEditExistingRecord(record);
    QApplication::processEvents();
    QVERIFY(passwordEdit->hasFocus());
    QTest::mouseClick(cancelButton, Qt::LeftButton);
    QApplication::processEvents();
    w.writableField = "NUMBER_EDIT";
    w.onEditExistingRecord(record);
    QApplication::processEvents();
    QVERIFY(spinBox->hasFocus());
    QTest::mouseClick(cancelButton, Qt::LeftButton);
    QApplication::processEvents();
    w.writableField = "COMBO_BOX";
    w.onEditExistingRecord(record);
    QApplication::processEvents();
    QVERIFY(comboBox->hasFocus());
    QTest::mouseClick(cancelButton, Qt::LeftButton);
    QApplication::processEvents();
    w.writableField = "";

    // Check for message box when validation fails.
    w.setValidator(&alwaysInvalidValidator);
    auto timerCallback = []() {
        QMessageBox *msgBox =
            dynamic_cast<QMessageBox *>(QApplication::activeModalWidget());
        QString title = "Invalid data entered";
        if (QSysInfo::kernelType() == "darwin") {
            // Required by OS X HIG.
            title = "";
        }
        QCOMPARE(msgBox->windowTitle(), title);
        QCOMPARE(msgBox->text(), QString("Nothing will ever be valid."));
        QCOMPARE(msgBox->detailedText(), QString("Just give up!"));
        QTest::keyClick(msgBox, Qt::Key_Enter);
    };
    QTimer::singleShot(200, timerCallback);
    w.onEditExistingRecord(record);
    QApplication::processEvents();
    QTest::mouseClick(saveButton, Qt::LeftButton);
    QApplication::processEvents();
}

QTEST_MAIN(TestWidgets)
