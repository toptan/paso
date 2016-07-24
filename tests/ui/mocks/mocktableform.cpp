#include "mocktableform.h"

#include "mockrecordeditorwidget.h"

#include <QSqlDatabase>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QTableView>
#include <utility>

using namespace std;

MockTableForm::MockTableForm() : TableForm(createModelAndEditor(), nullptr) {
    setupWidgets(new QTableView(this));
}

MockTableForm::~MockTableForm() {}

pair<QSqlTableModel *, RecordEditorWidget *>
MockTableForm::createModelAndEditor() {
    QSqlTableModel *model =
        new QSqlTableModel(nullptr, QSqlDatabase::database("paso"));
    model->setTable("course");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
    RecordEditorWidget *editor =
        new MockRecordEditorWidget(FieldTypes{{"CODE", FieldType::LineEdit},
                                              {"NAME", FieldType::LineEdit}},
                                   nullptr);
    editor->setupUi({{"CODE", "Code"}, {"NAME", "Name"}}, model->record());
    pair<QSqlTableModel *, RecordEditorWidget *> retVal;
    retVal.first = model;
    retVal.second = editor;
    return retVal;
}

void MockTableForm::setReadOnly(bool readOnly) { mReadOnly = readOnly; }

QSqlRecord MockTableForm::getSelectedRecord() const { return selectedRecord(); }

void MockTableForm::prepareRecordForSaving(QSqlRecord &record) {
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

bool MockTableForm::shouldEnableEditAction(const QSqlRecord &record) const {
    return true;
}

bool MockTableForm::shouldEnableDeleteAction(const QSqlRecord &record) const {
    return true;
}

bool MockTableForm::shouldDeleteRecord(const QSqlRecord &record) const {
    return !mReadOnly;
}

void MockTableForm::updateActions(const QSqlRecord &record) {}
