#include "studentform.h"
#include "ui_studentform.h"

#include "studentquerymodel.h"
#include "studenteditorwidget.h"
#include "studentvalidator.h"

#include "pasodb.h"

#include <QDebug>
#include <QSortFilterProxyModel>

using namespace paso::db;
using namespace std;

namespace paso {
namespace admin {

StudentForm::StudentForm(QWidget *parent)
    : QueryForm(createModelAndEditor(), parent), ui(new Ui::StudentForm) {
    ui->setupUi(this);
    setupWidgets(ui->tableView);
    ui->tableView->hideColumn(0);

    ui->horizontalLayout->addWidget(recordEditor());
    ui->horizontalLayout->setStretch(0, 3);
    ui->horizontalLayout->setStretch(1, 1);
    auto separator = new QAction(this);
    separator->setSeparator(true);
    mImportAction = new QAction(tr("Import"), this);
    toolBarActions().append(separator);
    toolBarActions().append(mImportAction);

    connect(mImportAction, &QAction::triggered, this, &StudentForm::onImport);
}

StudentForm::~StudentForm() { delete ui; }

std::pair<QSqlQueryModel *, RecordEditorWidget *>
StudentForm::createModelAndEditor() {
    QVariantMap columnLabels = {
        {"first_name", QObject::tr("First Name")},
        {"last_name", QObject::tr("Last Name")},
        {"email", QObject::tr("Email")},
        {"rfid", QObject::tr("RFID")},
        {"index_number", QObject::tr("Index Number")},
        {"year_of_study", QObject::tr("Year of Study")}};

    StudentQueryModel *model = new StudentQueryModel(
        columnLabels, QSqlDatabase::database(DEFAULT_DB_NAME));
    FieldTypes fieldTypes = {{"first_name", FieldType::LineEdit},
                             {"last_name", FieldType::LineEdit},
                             {"email", FieldType::LineEdit},
                             {"rfid", FieldType::LineEdit},
                             {"index_number", FieldType::MaskedLineEdit},
                             {"year_of_study", FieldType::NumberEdit}};
    auto editor = new StudentEditorWidget(model->record(), fieldTypes);
    editor->setupUi(columnLabels, model->record());
    editor->setValidator(new StudentValidator(editor->fieldTypes(),
                                              editor->fieldEditors(), editor));
    editor->clearData();

    return make_pair<QSqlQueryModel *, RecordEditorWidget *>(model, editor);
}

void StudentForm::prepareRecordForSaving(QSqlRecord &record) {}

bool StudentForm::shouldEnableEditAction(const QSqlRecord &record) const {
    return !record.isEmpty();
}

bool StudentForm::shouldEnableDeleteAction(const QSqlRecord &record) const {
    return !record.isEmpty();
}

bool StudentForm::shouldDeleteRecord(const QSqlRecord &record) const {
    // TODO: Check if something is connected with the student first!
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(
        tr("Do you want to delete selected student from the system?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    return msgBox.exec() == QMessageBox::Yes;
}

bool StudentForm::insertRecord(const QSqlRecord &record, QSqlError &error) {
    auto map = DBManager::recordToVariantMap(record);
    map.remove("id");
    Student student(map);
    if (manager().saveStudent(student, error)) {
        refreshModel();
        return true;
    }
    return false;
}

bool StudentForm::updateRecord(int, const QSqlRecord &record,
                               QSqlError &error) {
    auto map = DBManager::recordToVariantMap(record);
    Student student(map);
    if (manager().saveStudent(student, error)) {
        refreshModel();
        return true;
    }
    return false;
}

void StudentForm::onImport() {
    // TODO: Implement students import.
}
}
}