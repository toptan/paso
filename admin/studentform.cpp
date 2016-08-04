#include "studentform.h"
#include "ui_studentform.h"

#include "logdialog.h"
#include "studenteditorwidget.h"
#include "studentquerymodel.h"
#include "studentvalidator.h"

#include "pasodb.h"

#include <QFile>
#include <QFileDialog>
#include <QSqlError>
#include <QTextStream>
#include <QtConcurrent>

using namespace paso::db;
using namespace paso::model;
using namespace paso::widget;
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
    mImportAction->setObjectName("IMPORT_ACTION");
    toolBarActions().append(separator);
    toolBarActions().append(mImportAction);
    separator = new QAction(this);
    separator->setSeparator(true);
    mDetailsAction = new QAction(tr("Details"), this);
    mDetailsAction->setObjectName("DETAILS_ACTION");
    toolBarActions().append(separator);
    toolBarActions().append(mDetailsAction);
    mDetailsAction->setEnabled(false);
    connect(mImportAction, &QAction::triggered, this, &StudentForm::onImport);
    // TODO: Add student details
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
    auto editor = new StudentEditorWidget(fieldTypes);
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

void StudentForm::updateActions(const QSqlRecord &record) {
    mDetailsAction->setEnabled(record.value("id") != 0);
}

bool StudentForm::insertRecord(QSqlRecord &record, QSqlError &error) {
    auto map = DBManager::recordToVariantMap(record);
    map.remove("id");
    Student student(map);
    auto success = manager().saveStudent(student, error);
    if (success) {
        refreshModel();
        record.setValue("id", static_cast<quint64>(student.id()));
    }
    return success;
}

bool StudentForm::updateRecord(int, const QSqlRecord &record,
                               QSqlError &error) {
    auto map = DBManager::recordToVariantMap(record);
    Student student(map);
    auto success = manager().saveStudent(student, error);
    if (success) {
        refreshModel();
    }
    return success;
}

bool StudentForm::removeRow(int row, QSqlError &error) {
    auto indexNumber = model()->record(row).value("index_number").toString();
    auto success = manager().deleteStudent(indexNumber, error);
    if (success) {
        refreshModel();
    }
    return success;
}

void StudentForm::onImport() {
    auto fileName = QFileDialog::getOpenFileName(
        this, tr("Open students import file"), "", "*.csv");
    auto file = new QFile(fileName);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox msgBox(this);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.setIcon(QMessageBox::Critical);
        QString text =
            QString(tr("The file %1 cannot be opened.")).arg(fileName);
        msgBox.setText(text);
        msgBox.setDetailedText(file->errorString());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        delete file;
        return;
    }

    auto logDialog = new LogDialog(tr("Importing students..."));
    logDialog->setModal(true);
    logDialog->show();
    connect(logDialog, &LogDialog::accepted, logDialog,
            &LogDialog::deleteLater);
    connect(logDialog, &LogDialog::rejected, logDialog,
            &LogDialog::deleteLater);
    connect(this, &StudentForm::newLogLine, logDialog, &LogDialog::appendLine);
    connect(this, &StudentForm::importDone, logDialog,
            &LogDialog::processingDone);

    auto work = [this, file, logDialog]() {
        QTextStream in(file);
        bool errorOccured = false;
        int lineNo = 1;
        db::DBManager manager;
        QSqlError sqlError;
        QString format("Importing line %1... %2");
        emit newLogLine(
            QString(tr("Importing students from %1")).arg(file->fileName()));
        emit newLogLine("");
        while (!in.atEnd()) {
            QString message;
            QString line = in.readLine();
            auto error = manager.importStudent(line, sqlError);

            switch (error) {
            case StudentImportError::NO_ERROR:
                message = format.arg(lineNo).arg(tr("OK."));
                break;
            case StudentImportError::INVALID_LINE:
                message = format.arg(lineNo).arg(
                    tr("The student data must be properly comma separated."));
                errorOccured = true;
                break;
            case StudentImportError::NO_INDEX_NUMBER:
                message =
                    format.arg(lineNo).arg(tr("The index number is missing."));
                errorOccured = true;
                break;
            case StudentImportError::BAD_INDEX_NUMBER:
                message = format.arg(lineNo).arg(
                    tr("The index number is in wrong format."));
                errorOccured = true;
                break;
            case StudentImportError::NO_FIRST_NAME:
                message = format.arg(lineNo).arg(
                    tr("The student's first name is missing."));
                errorOccured = true;
                break;
            case StudentImportError::FIRST_NAME_TOO_LONG:
                message = format.arg(lineNo).arg(
                    tr("The student's first name exceeds 32 characters."));
                errorOccured = true;
                break;
            case StudentImportError::NO_LAST_NAME:
                message = format.arg(lineNo).arg(
                    tr("The student's last name is missing."));
                errorOccured = true;
                break;
            case StudentImportError::LAST_NAME_TOO_LONG:
                message = format.arg(lineNo).arg(
                    tr("The student's last name exceeds 32 characters."));
                errorOccured = true;
                break;
            case StudentImportError::BAD_EMAIL:
                message = format.arg(lineNo).arg(
                    tr("The student's email is illformed."));
                errorOccured = true;
                break;
            case StudentImportError::NO_YEAR_OF_STUDY:
                message = format.arg(lineNo).arg(
                    tr("The student's year of study is missing."));
                errorOccured = true;
                break;
            case StudentImportError::BAD_YEAR_OF_STUDY:
                message = format.arg(lineNo).arg(
                    tr("The student's year of study is wrong."));
                errorOccured = true;
                break;
            case StudentImportError::DB_ERROR:
                message = format.arg(lineNo).arg(sqlError.text());
                errorOccured = true;
                break;
            }
            emit newLogLine(message);
            lineNo++;
        }
        delete file;
        if (!errorOccured) {
            emit newLogLine("");
            emit newLogLine(tr("Import finished without errors."));
        } else {
            emit newLogLine("");
            emit newLogLine(tr(
                "Not all lines could be imported. Please see messages above."));
        }
        refreshModel();
        emit importDone();
    };

    QtConcurrent::run(work);
}
}
}
