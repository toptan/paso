#include "courseform.h"
#include "ui_courseform.h"

#include "data.h"
#include "pasodb.h"
#include "recordeditorwidget.h"
#include "coursevalidator.h"
#include "courseeditorwidget.h"

#include <QSqlField>

using namespace paso::db;
using namespace paso::data;

using namespace std;

namespace paso {
namespace admin {

CourseForm::CourseForm(QWidget *parent)
    : AbstractForm(createModelAndEditor(), parent), ui(new Ui::CourseForm) {
    ui->setupUi(this);
    setupWidgets(ui->tableView);

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
    QMessageBox::information(this, "Info", "Importing...");
}

}
}
