#include "courseform.h"
#include "ui_courseform.h"

#include "data.h"
#include "course.h"
#include "pasodb.h"
#include "recordeditorwidget.h"
#include "coursevalidator.h"
#include "courseeditorwidget.h"
#include "logdialog.h"
#include "coursedetailsdialog.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QSqlError>
#include <QSqlField>
#include <QtConcurrent>
#include <QTextStream>
#include <QThread>

using namespace paso::db;
using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::widget;

using namespace std;

namespace paso {
namespace admin {

CourseForm::CourseForm(QWidget *parent)
    : TableForm(createModelAndEditor(), parent), ui(new Ui::CourseForm) {
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
    separator = new QAction(this);
    separator->setSeparator(true);
    mDetailsAction = new QAction(tr("Details"), this);
    toolBarActions().append(separator);
    toolBarActions().append(mDetailsAction);
    mDetailsAction->setEnabled(false);

    connect(mImportAction, &QAction::triggered, this, &CourseForm::onImport);
    connect(mDetailsAction, &QAction::triggered, this, &CourseForm::onDetails);
}

CourseForm::~CourseForm() { delete ui; }

pair<QSqlTableModel *, RecordEditorWidget *>
CourseForm::createModelAndEditor() {
    QVariantMap columnLabels = {
        {"code", QObject::tr("Code")}, {"name", QObject::tr("Course")},
    };

    auto model = new CourseTableModel(columnLabels,
                                      QSqlDatabase::database(DEFAULT_DB_NAME));
    FieldTypes fieldTypes = {{"code", FieldType::LineEdit},
                             {"name", FieldType::LineEdit}};
    auto editor = new CourseEditorWidget(model->record(), fieldTypes);
    editor->setupUi(columnLabels, model->record());
    editor->setValidator(new CourseValidator(editor->fieldTypes(),
                                             editor->fieldEditors(), editor));
    editor->clearData();

    return make_pair<QSqlTableModel *, RecordEditorWidget *>(model, editor);
}

void CourseForm::prepareRecordForSaving(QSqlRecord &record) {
    // If value of the ID field is null, we need to remove it to let database
    // assign a value.
    auto index = record.indexOf("ID");
    if (index == -1) {
        return;
    }
    if (record.field(index).isNull()) {
        record.remove(index);
    }
}

bool CourseForm::shouldEnableEditAction(const QSqlRecord &record) const {
    return !record.isEmpty();
}

bool CourseForm::shouldEnableDeleteAction(const QSqlRecord &record) const {
    return !record.isEmpty();
}

bool CourseForm::shouldDeleteRecord(const QSqlRecord &record) const {
    // TODO: Check if something is connected with the course first!
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(
        tr("Do you want to delete selected course from the system?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    return msgBox.exec() == QMessageBox::Yes;
}

void CourseForm::updateActions(const QSqlRecord &record) {
    mDetailsAction->setEnabled(record.value("id") != 0);
}

void CourseForm::onImport() {
    auto fileName = QFileDialog::getOpenFileName(
        this, tr("Open courses import file"), "", "*.csv");
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

    auto logDialog = new LogDialog(tr("Importing course data..."));
    logDialog->setModal(true);
    logDialog->show();
    connect(logDialog, &LogDialog::accepted, logDialog,
            &LogDialog::deleteLater);
    connect(logDialog, &LogDialog::rejected, logDialog,
            &LogDialog::deleteLater);
    connect(this, &CourseForm::newLogLine, logDialog, &LogDialog::appendLine);
    connect(this, &CourseForm::importDone, logDialog,
            &LogDialog::processingDone);

    auto work = [this, file, logDialog]() {
        QTextStream in(file);
        bool errorOccured = false;
        int lineNo = 1;
        db::DBManager manager;
        QSqlError sqlError;
        QString format(tr("Importing line %1... %2"));
        emit newLogLine(
            QString(tr("Importing course data from %1")).arg(file->fileName()));
        emit newLogLine("");
        while (!in.atEnd()) {
            QString message;
            QString line = in.readLine();
            auto error = manager.importCourse(line, sqlError);

            switch (error) {
            case CourseImportError::NO_ERROR:
                message = format.arg(lineNo).arg(tr("OK."));
                break;
            case CourseImportError::INVALID_LINE:
                message = format.arg(lineNo).arg(
                    tr("The course code and name must be comma separated."));
                errorOccured = true;
                break;
            case CourseImportError::NO_CODE:
                message =
                    format.arg(lineNo).arg(tr("The course code is missing."));
                errorOccured = true;
                break;
            case CourseImportError::CODE_TOO_LONG:
                message = format.arg(lineNo)
                              .arg(tr("The course code exceeds 8 characters."));
                errorOccured = true;
                break;
            case CourseImportError::NO_NAME:
                message =
                    format.arg(lineNo).arg(tr("The course name is missing."));
                errorOccured = true;
                break;
            case CourseImportError::NAME_TOO_LONG:
                message = format.arg(lineNo).arg(
                    tr("The course name exceeds 64 characters."));
                errorOccured = true;
                break;
            case CourseImportError::DB_ERROR:
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

void CourseForm::onDetails() {
    Course course(DBManager::recordToVariantMap(selectedRecord()));
    CourseDetailsDialog dlg(course, this);
    dlg.exec();
}
}
}
