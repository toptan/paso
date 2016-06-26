#include "courseform.h"
#include "ui_courseform.h"

#include "data.h"
#include "pasodb.h"
#include "recordeditorwidget.h"
#include "coursevalidator.h"
#include "courseeditorwidget.h"
#include "logdialog.h"

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

using namespace std;

namespace paso {
namespace admin {

CourseForm::CourseForm(QWidget *parent)
    : AbstractForm(createModelAndEditor(), parent), ui(new Ui::CourseForm) {
    ui->setupUi(this);
    setupWidgets(ui->tableView);
    ui->tableView->hideColumn(0);

    ui->horizontalLayout->addWidget(recordEditor());
    ui->horizontalLayout->setStretch(0, 2);
    ui->horizontalLayout->setStretch(1, 1);
    auto separator = new QAction(this);
    separator->setSeparator(true);
    mImportAction = new QAction(tr("Import"), this);
    toolBarActions().append(separator);
    toolBarActions().append(mImportAction);

    connect(mImportAction, &QAction::triggered, this, &CourseForm::onImport);
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

    auto logDialog = new LogDialog();
    logDialog->setModal(true);
    logDialog->show();
    connect(logDialog, &LogDialog::accepted, logDialog,
            &LogDialog::deleteLater);
    connect(logDialog, &LogDialog::rejected, logDialog,
            &LogDialog::deleteLater);
    connect(this, &CourseForm::newLogLine, logDialog, &LogDialog::appendLine);

    auto work = [this, file, logDialog]() {
        QTextStream in(file);
        int lineNo = 1;
        db::DBManager manager;
        QSqlError sqlError;
        QString format("Importing line %1... %2");

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
                break;
            case CourseImportError::NO_CODE:
                message =
                    format.arg(lineNo).arg(tr("The course code is missing."));
                break;
            case CourseImportError::CODE_TOO_LONG:
                message = format.arg(lineNo)
                              .arg(tr("The course code exceeds 8 characters."));
                break;
            case CourseImportError::NO_NAME:
                message =
                    format.arg(lineNo).arg(tr("The course name is missing."));
                break;
            case CourseImportError::NAME_TOO_LONG:
                message = format.arg(lineNo).arg(
                    tr("The course name exceeds 64 characters."));
                break;
            case CourseImportError::DB_ERROR:
                message = format.arg(lineNo).arg(sqlError.text());
                break;
            }
            emit newLogLine(message);
            QThread::currentThread()->msleep(200);
            lineNo++;
        }
        delete file;
    };

    QtConcurrent::run(work);
}
}
}
