#include "teacherform.h"
#include "ui_teacherform.h"

#include "entryreportdialog.h"
#include "pasodb.h"
#include "teachereditorwidget.h"
#include "teacherquerymodel.h"
#include "teachervalidator.h"

#include <QSqlError>

using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::db;
using namespace paso::model;
using namespace paso::widget;
using namespace std;

namespace paso {
namespace admin {

TeacherForm::TeacherForm(QWidget *parent)
    : QueryForm(createModelAndEditor(), parent), ui(new Ui::TeacherForm) {
    ui->setupUi(this);
    setupWidgets(ui->tableView);
    ui->tableView->hideColumn(0);

    ui->horizontalLayout->addWidget(recordEditor());
    ui->horizontalLayout->setStretch(0, 3);
    ui->horizontalLayout->setStretch(1, 1);

    auto separator = new QAction(this);
    separator->setSeparator(true);
    mReportAction = new QAction(tr("Entries"), this);
    mReportAction->setObjectName("REPORT_ACTION");
    toolBarActions().append(separator);
    toolBarActions().append(mReportAction);

    connect(mReportAction, &QAction::triggered, this, &TeacherForm::onReport);
    mReportAction->setEnabled(false);
}

TeacherForm::~TeacherForm() { delete ui; }

std::pair<QSqlQueryModel *, RecordEditorWidget *>
TeacherForm::createModelAndEditor() {
    QVariantMap columnLabels = {
        {"first_name", QObject::tr("First Name")},
        {"last_name", QObject::tr("Last Name")},
        {"email", QObject::tr("Email")},
        {"rfid", QObject::tr("RFID")},
        {"employee_number", QObject::tr("Employee Number")},
        {"office", QObject::tr("Office Number")}};

    TeacherQueryModel *model = new TeacherQueryModel(
        columnLabels, QSqlDatabase::database(DEFAULT_DB_NAME));
    FieldTypes fieldTypes = {{"first_name", FieldType::LineEdit},
                             {"last_name", FieldType::LineEdit},
                             {"email", FieldType::LineEdit},
                             {"rfid", FieldType::LineEdit},
                             {"employee_number", FieldType::LineEdit},
                             {"office", FieldType::LineEdit}};
    auto editor = new TeacherEditorWidget(fieldTypes);
    editor->setupUi(columnLabels, model->record());
    editor->setValidator(new TeacherValidator(editor->fieldTypes(),
                                              editor->fieldEditors(), editor));
    editor->clearData();

    return make_pair<QSqlQueryModel *, RecordEditorWidget *>(model, editor);
}

void TeacherForm::prepareRecordForSaving(QSqlRecord &record) {}

bool TeacherForm::shouldEnableEditAction(const QSqlRecord &record) const {
    return !record.isEmpty();
}

bool TeacherForm::shouldEnableDeleteAction(const QSqlRecord &record) const {
    return !record.isEmpty();
}

bool TeacherForm::shouldDeleteRecord(const QSqlRecord &record) const {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(
        tr("Do you want to delete selected teacher from the system?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    return msgBox.exec() == QMessageBox::Yes;
}

void TeacherForm::updateActions(const QSqlRecord &record) {
    // No actions to update
    mReportAction->setEnabled(!record.isEmpty());
}

bool TeacherForm::insertRecord(QSqlRecord &record, QSqlError &error) {
    auto map = DBManager::recordToVariantMap(record);
    map.remove("id");
    Teacher teacher(map);
    auto success = manager().saveTeacher(teacher, error);
    if (success) {
        refreshModel();
        record.setValue("id", teacher.id());
    }
    return success;
}

bool TeacherForm::updateRecord(int, const QSqlRecord &record,
                               QSqlError &error) {
    auto map = DBManager::recordToVariantMap(record);
    Teacher teacher(map);
    auto success = manager().saveTeacher(teacher, error);
    if (success) {
        refreshModel();
    }
    return success;
}

bool TeacherForm::removeRow(int row, QSqlError &error) {
    auto employeeNumber =
        model()->record(row).value("employee_number").toString();
    auto success = manager().deleteTeacher(employeeNumber, error);
    if (success) {
        refreshModel();
    }
    return success;
}

void TeacherForm::onReport() {
    EntryReportDialog dlg(this);
    dlg.setWindowTitle(
        tr("%1 %2 (%3) - room entries report")
            .arg(selectedRecord().value("last_name").toString(),
                 selectedRecord().value("first_name").toString(),
                 selectedRecord().value("employee_number").toString()));
    dlg.setPersonNumber(selectedRecord().value("employee_number").toString());
    dlg.exec();
}
}
}
