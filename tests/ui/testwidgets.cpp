#include "testwidgets.h"

#include "addremoveentitiesform.h"
#include "course.h"
#include "entity.h"
#include "itemspicker.h"
#include "pdateedit.h"
#include "recordeditorwidget.h"

#include "mocks/mockalwaysinvalidrecordvalidator.h"
#include "mocks/mockalwaysvalidrecordvalidator.h"
#include "mocks/mockrecordeditorwidget.h"
#include "mocks/mocktableform.h"

#include <QAbstractTableModel>
#include <QCheckBox>
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

TestWidgets::TestWidgets() : TestBase() {}

void TestWidgets::testAddRemoveEntityWidget() {
    AddRemoveEntitiesForm form;
    QStringList columns{"name", "code"};
    QMap<QString, QString> columnNames{{"name", "Predmet"}, {"code", "Šifra"}};

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

    form.setReadOnly();
    QVERIFY(form.readOnly());
    QVERIFY(!addButton->isEnabled());
    QVERIFY(!removeButton->isEnabled());
    QVERIFY(!resetButton->isEnabled());

    form.setReadOnly(false);
    QVERIFY(!form.readOnly());
    QVERIFY(addButton->isEnabled());
    QVERIFY(removeButton->isEnabled());
    QVERIFY(resetButton->isEnabled());

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

    int addedIdsSize = 0;
    connect(&form, &AddRemoveEntitiesForm::addedEntitiesChanged,
            [&addedIdsSize](QList<quint64> l) { addedIdsSize = l.size(); });
    int removedIdsSize = 0;
    connect(&form, &AddRemoveEntitiesForm::removedEntitiesChanged,
            [&removedIdsSize](QList<quint64> l) { removedIdsSize = l.size(); });

    QSignalSpy spyForAdded(&form, &AddRemoveEntitiesForm::addedEntitiesChanged);
    QSignalSpy spyForRemoved(&form,
                             &AddRemoveEntitiesForm::removedEntitiesChanged);
    QSignalSpy spyForDestinationEntitiesChanged(
        &form, &AddRemoveEntitiesForm::destinationEntitiesChanged);
    QPoint point(sourceTable->columnViewportPosition(0) + 5,
                 sourceTable->rowViewportPosition(0) + 10);
    auto pViewport = sourceTable->viewport();
    QTest::mouseClick(pViewport, Qt::LeftButton, 0, point);
    QTest::mouseDClick(pViewport, Qt::LeftButton, 0, point);
    QApplication::processEvents();
    QCOMPARE(form.addedEntities().size(), size_t(1));
    QCOMPARE(form.removedEntities().size(), size_t(1));
    QCOMPARE(spyForAdded.count(), 1);
    QCOMPARE(spyForRemoved.count(), 1);
    QCOMPARE(spyForDestinationEntitiesChanged.count(), 1);
    QCOMPARE(addedIdsSize, 1);
    QCOMPARE(removedIdsSize, 1);

    point.setX(destinationTable->columnViewportPosition(0) + 5);
    point.setY(destinationTable->rowViewportPosition(0) + 10);
    pViewport = destinationTable->viewport();
    QTest::mouseClick(pViewport, Qt::LeftButton, 0, point);
    QTest::mouseDClick(pViewport, Qt::LeftButton, 0, point);
    QApplication::processEvents();
    QVERIFY(form.addedEntities().empty());
    QCOMPARE(form.removedEntities().size(), size_t(2));
    QCOMPARE(spyForAdded.count(), 2);
    QCOMPARE(spyForRemoved.count(), 2);
    QCOMPARE(spyForDestinationEntitiesChanged.count(), 2);
    QCOMPARE(addedIdsSize, 0);
    QCOMPARE(removedIdsSize, 2);
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
    FieldTypes fieldTypes{{"line_edit", FieldType::LineEdit},
                          {"masked_line_edit", FieldType::MaskedLineEdit},
                          {"password_edit", FieldType::PasswordEdit},
                          {"number_edit", FieldType::NumberEdit},
                          {"combo_box", FieldType::ComboBox},
                          {"check_box", FieldType::CheckBox},
                          {"date_edit", FieldType::DateEdit},
                          {"time_edit", FieldType::TimeEdit}};

    QSqlRecord record;
    record.append(QSqlField("line_edit", QVariant::String));
    record.append(QSqlField("masked_line_edit", QVariant::String));
    record.append(QSqlField("password_edit", QVariant::String));
    record.append(QSqlField("id", QVariant::LongLong));
    record.append(QSqlField("number_edit", QVariant::Int));
    record.append(QSqlField("combo_box", QVariant::Int));
    record.append(QSqlField("check_box", QVariant::Bool));
    record.append(QSqlField("date_edit", QVariant::Date));
    record.append(QSqlField("time_edit", QVariant::Time));

    QVariantMap columnLabels{{"line_edit", "LineEdit"},
                             {"masked_line_edit", "MaskedLineEdit"},
                             {"password_edit", "PasswordEdit"},
                             {"number_edit", "NumberEdit"},
                             {"combo_box", "ComboBox"},
                             {"check_box", "CheckBox"},
                             {"date_edit", "DateEdit"},
                             {"time_edit", "TimeEdit"}};

    const QDate testDate = QDate::currentDate().addMonths(6);
    const QTime testTime = QTime::currentTime();

    MockAlwaysValidRecordValidator alwaysValidValidator;
    MockAlwaysInvalidRecordValidator alwaysInvalidValidator;
    MockRecordEditorWidget w(fieldTypes);

    w.setupUi(columnLabels, record);
    w.setValidator(&alwaysValidValidator);
    w.show();
    QVERIFY(QTest::qWaitForWindowActive(&w, 10000));
    QVERIFY(QTest::qWaitForWindowExposed(&w, 10000));
    // Form layout, button box + 2 x (columns - ID column)
    QCOMPARE(w.children().size(), 18);
    QCOMPARE(w.fieldTypes(), fieldTypes);
    QCOMPARE(w.fieldEditors().keys(), fieldTypes.keys());
    record.setValue("id", 42);
    record.setValue("line_edit", "Line Edit");
    record.setValue("masked_line_edit", "Masked/Line/Edit");
    record.setValue("password_edit", "Password Edit");
    record.setValue("number_edit", 9);
    record.setValue("combo_box", 2);
    record.setValue("check_box", true);
    record.setValue("date_edit", testDate);
    record.setValue("time_edit", testTime);
    QVariantList stringValues{"Line Edit", "Masked/Line/Edit", "Password Edit"};
    w.onDisplayRecord(record);
    QApplication::processEvents();
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
    auto checkBox = w.findChild<QCheckBox *>();
    QVERIFY(checkBox->isChecked());
    QVERIFY(!checkBox->isEnabled());
    auto dateEdit = w.findChild<QDateEdit *>();
    QVERIFY(dateEdit->isReadOnly());
    auto timeEdit = w.findChild<QTimeEdit *>();
    QVERIFY(timeEdit->isReadOnly());
    QCOMPARE(timeEdit->displayFormat(), QString("H.mm"));

    auto buttonBox = w.findChild<QDialogButtonBox *>();
    QVERIFY(!buttonBox->isVisible());
    QCOMPARE(w.fieldEditors().value("line_edit"), lineEdit);
    QCOMPARE(w.fieldEditors().value("masked_line_edit"), maskedLineEdit);
    QCOMPARE(w.fieldEditors().value("password_edit"), passwordEdit);
    QCOMPARE(w.fieldEditors().value("number_edit"), spinBox);
    QCOMPARE(w.fieldEditors().value("combo_box"), comboBox);
    QCOMPARE(w.fieldEditors().value("check_box"), checkBox);
    QCOMPARE(w.fieldEditors().value("date_edit"), dateEdit);
    QCOMPARE(w.fieldEditors().value("time_edit"), timeEdit);
    auto saveButton = buttonBox->button(QDialogButtonBox::Save);
    auto cancelButton = buttonBox->button(QDialogButtonBox::Cancel);
    QApplication::processEvents();

    w.clearData();
    QApplication::processEvents();
    QVERIFY(lineEdit->text().isEmpty());
    QCOMPARE(maskedLineEdit->text(), QString("//"));
    QVERIFY(passwordEdit->text().isEmpty());
    QCOMPARE(spinBox->value(), spinBox->minimum());
    QVERIFY(spinBox->text().isEmpty());
    QCOMPARE(comboBox->currentIndex(), -1);
    QVERIFY(!checkBox->isChecked());
    QCOMPARE(dateEdit->date(), QDate::currentDate());
    QCOMPARE(timeEdit->time(), QTime(0, 0, 0));

    w.clearData();
    QApplication::processEvents();
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
    QVERIFY(checkBox->isChecked());
    QVERIFY(checkBox->isEnabled());
    QCOMPARE(dateEdit->date(), testDate);
    QVERIFY(!dateEdit->isReadOnly());
    QCOMPARE(timeEdit->time(), testTime);
    QVERIFY(!timeEdit->isReadOnly());
    QVERIFY(buttonBox->isVisible());

    QTest::mouseClick(saveButton, Qt::LeftButton);
    QApplication::processEvents();
    w.saveSuccessfull();
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
    QVERIFY(checkBox->isChecked());
    QVERIFY(!checkBox->isEnabled());
    QCOMPARE(dateEdit->date(), testDate);
    QVERIFY(dateEdit->isReadOnly());
    QCOMPARE(timeEdit->time(), testTime);
    QVERIFY(timeEdit->isReadOnly());
    QVERIFY(!buttonBox->isVisible());

    w.onEditExistingRecord(record);
    QApplication::processEvents();
    lineEdit->setText("FOO");
    maskedLineEdit->setText("BBBBBB/CCCC/DDDD");
    passwordEdit->setText("BAR");
    spinBox->setValue(10);
    comboBox->setCurrentIndex(2);
    checkBox->toggle();
    dateEdit->setDate(dateEdit->date().addDays(2));
    timeEdit->setTime(QTime(1, 30, 0));
    QApplication::processEvents();
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
    QVERIFY(checkBox->isChecked());
    QVERIFY(!checkBox->isEnabled());
    QCOMPARE(dateEdit->date(), testDate);
    QVERIFY(dateEdit->isReadOnly());
    QCOMPARE(timeEdit->time(), testTime);
    QVERIFY(timeEdit->isReadOnly());
    QVERIFY(!buttonBox->isVisible());

    w.clearData();
    record.clearValues();
    QApplication::processEvents();
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
    QVERIFY(!checkBox->isChecked());
    QVERIFY(checkBox->isEnabled());
    QCOMPARE(dateEdit->date(), QDate::currentDate());
    QVERIFY(!dateEdit->isReadOnly());
    QCOMPARE(timeEdit->time(), QTime(0, 0, 0));
    QVERIFY(!timeEdit->isReadOnly());
    QVERIFY(buttonBox->isVisible());
    QVERIFY(lineEdit->hasFocus());

    lineEdit->setText("FOO");
    maskedLineEdit->setText("BBBBBB/CCCC/DDDD");
    passwordEdit->setText("BAR");
    spinBox->setValue(10);
    comboBox->setCurrentIndex(2);
    checkBox->toggle();
    QApplication::processEvents();
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
    QVERIFY(!checkBox->isChecked());
    QVERIFY(!checkBox->isEnabled());
    QVERIFY(!buttonBox->isVisible());

    lineEdit->setText("FOO");
    maskedLineEdit->setText("BBBBBB/CCCC/DDDD");
    passwordEdit->setText("BAR");
    spinBox->setValue(10);
    comboBox->setCurrentIndex(2);
    checkBox->setChecked(true);
    dateEdit->setDate(testDate.addDays(2));
    timeEdit->setTime(testTime.addSecs(120));
    QApplication::processEvents();
    QTest::mouseClick(saveButton, Qt::LeftButton);
    QApplication::processEvents();
    w.saveSuccessfull();
    QApplication::processEvents();
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
    QVERIFY(checkBox->isChecked());
    QVERIFY(!checkBox->isEnabled());
    QCOMPARE(dateEdit->date(), testDate.addDays(2));
    QVERIFY(dateEdit->isReadOnly());
    QCOMPARE(timeEdit->time(), testTime.addSecs(120));
    QVERIFY(timeEdit->isReadOnly());
    QVERIFY(!buttonBox->isVisible());

    // Focus checks
    w.writableField = "masked_line_edit";
    w.onEditExistingRecord(record);
    QApplication::processEvents();
    QVERIFY(maskedLineEdit->hasFocus());
    QTest::mouseClick(cancelButton, Qt::LeftButton);
    QApplication::processEvents();
    w.writableField = "password_edit";
    w.onEditExistingRecord(record);
    QApplication::processEvents();
    QVERIFY(passwordEdit->hasFocus());
    QTest::mouseClick(cancelButton, Qt::LeftButton);
    QApplication::processEvents();
    w.writableField = "number_edit";
    w.onEditExistingRecord(record);
    QApplication::processEvents();
    QVERIFY(spinBox->hasFocus());
    QTest::mouseClick(cancelButton, Qt::LeftButton);
    QApplication::processEvents();
    w.writableField = "combo_box";
    w.onEditExistingRecord(record);
    QApplication::processEvents();
    QVERIFY(comboBox->hasFocus());
    QTest::mouseClick(cancelButton, Qt::LeftButton);
    QApplication::processEvents();
    w.writableField = "check_box";
    w.onEditExistingRecord(record);
    QApplication::processEvents();
    QVERIFY(checkBox->hasFocus());
    QTest::mouseClick(cancelButton, Qt::LeftButton);
    QApplication::processEvents();
    w.writableField = "date_edit";
    w.onEditExistingRecord(record);
    QApplication::processEvents();
    QVERIFY(dateEdit->hasFocus());
    QTest::mouseClick(cancelButton, Qt::LeftButton);
    QApplication::processEvents();
    w.writableField = "time_edit";
    w.onEditExistingRecord(record);
    QApplication::processEvents();
    QVERIFY(timeEdit->hasFocus());
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

void TestWidgets::testDateEditWidget() {
    PDateEdit dateEditWithoutDate;
    dateEditWithoutDate.show();
    QTest::qWaitForWindowExposed(&dateEditWithoutDate);
    QDate date(1977, 1, 5);
    dateEditWithoutDate.setDate(date);
    QApplication::processEvents();
    QCOMPARE(dateEditWithoutDate.text(),
             date.toString(dateEditWithoutDate.displayFormat()));
    QDate invalidDate(-1, -1, -1);
    dateEditWithoutDate.setDate(invalidDate);
    QApplication::processEvents();
    dateEditWithoutDate.setDate(date);
    QApplication::processEvents();
    QCOMPARE(dateEditWithoutDate.text(),
             date.toString(dateEditWithoutDate.displayFormat()));

    PDateEdit dateEditWithDate(invalidDate);
    dateEditWithDate.show();
    QTest::qWaitForWindowExposed(&dateEditWithDate);
    QCOMPARE(dateEditWithDate.text(), QString(""));
    dateEditWithDate.setDate(invalidDate);
    QApplication::processEvents();
    QCOMPARE(dateEditWithDate.text(), QString(""));
    dateEditWithDate.setDate(date);
    QApplication::processEvents();
    QCOMPARE(dateEditWithDate.text(),
             date.toString(dateEditWithDate.displayFormat()));
}

void TestWidgets::testItemsPicker() {
    ItemsPicker picker1(2);
    QStringList objectNames;
    objectNames << "item000"
                << "item001"
                << "item002"
                << "item003"
                << "item004";
    QStringList itemLabels;
    itemLabels << "000"
               << "001"
               << "002"
               << "003"
               << "004";
    picker1.show();
    QTest::qWaitForWindowExposed(&picker1);
    picker1.setItemLabels(itemLabels);
    QCOMPARE(picker1.columnCount(), 2);
    for (const auto &child : picker1.findChildren<QCheckBox *>()) {
        QVERIFY(objectNames.contains(child->objectName()));
        objectNames.removeOne(child->objectName());
    }
    QVERIFY(objectNames.empty());
    QSignalSpy spy(&picker1, &ItemsPicker::selectedItemsChanged);
    auto item001 =
        dynamic_cast<QCheckBox *>(picker1.findChild<QCheckBox *>("item001"));
    auto item003 =
        dynamic_cast<QCheckBox *>(picker1.findChild<QCheckBox *>("item003"));
    item001->setChecked(true);
    item003->setChecked(true);
    QApplication::processEvents();
    QCOMPARE(spy.count(), 2);
    auto selectedItems = picker1.selectedItems();
    QCOMPARE(selectedItems.size(), 2);
    // Selected items returned are 1 based not zero based!
    QVERIFY(selectedItems.contains(2));
    QVERIFY(selectedItems.contains(4));
    picker1.hide();
    QApplication::processEvents();

    ItemsPicker picker2(3);
    picker2.show();
    QTest::qWaitForWindowExposed(&picker2);
    picker2.setItemLabels(itemLabels);
    QApplication::processEvents();
    QVERIFY(picker2.selectedItems().empty());
    picker2.setSelectedItems({1, 4, 7});
    QApplication::processEvents();
    selectedItems = picker2.selectedItems();
    QCOMPARE(selectedItems.size(), 2);
    QVERIFY(selectedItems.contains(1));
    QVERIFY(selectedItems.contains(4));
    picker2.setColumnCount(2);
    QApplication::processEvents();
    selectedItems = picker2.selectedItems();
    QCOMPARE(selectedItems.size(), 2);
    QVERIFY(selectedItems.contains(1));
    QVERIFY(selectedItems.contains(4));
    picker2.setItemLabels({"A", "B", "C"});
    QApplication::processEvents();
    selectedItems = picker2.selectedItems();
    QVERIFY(selectedItems.empty());
    picker2.hide();
    QApplication::processEvents();
}
