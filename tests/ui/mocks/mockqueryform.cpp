#include "mockqueryform.h"

#include "mocks/mockrecordeditorwidget.h"
#include "refreshablesqlquerymodel.h"

#include <QTableView>

using namespace std;
using namespace paso::model;

MockQueryForm::MockQueryForm() : QueryForm(createModelAndEditor()) {
    setupWidgets(new QTableView(this));
}

MockQueryForm::~MockQueryForm() {}

pair<QSqlQueryModel *, RecordEditorWidget *>
MockQueryForm::createModelAndEditor() {
    QVariantMap columnLabels{{"code", "Code"}, {"name", "Name"}};
    auto model = new RefreshableSqlQueryModel("SELECT * FROM COURSE", "paso");
    model->select();
    RecordEditorWidget *editor =
        new MockRecordEditorWidget(FieldTypes{{"code", FieldType::LineEdit},
                                              {"name", FieldType::LineEdit}},
                                   nullptr);
    editor->setupUi({{"code", "Code"}, {"name", "Name"}}, model->record());

    pair<QSqlQueryModel *, RecordEditorWidget *> retVal;
    retVal.first = model;
    retVal.second = editor;
    return retVal;
}

void MockQueryForm::prepareRecordForSaving(QSqlRecord &record) {}

bool MockQueryForm::shouldEnableEditAction(const QSqlRecord &record) const {
    return true;
}

bool MockQueryForm::shouldEnableDeleteAction(const QSqlRecord &record) const {
    return true;
}

bool MockQueryForm::shouldDeleteRecord(const QSqlRecord &record) const {
    return true;
}

void MockQueryForm::updateActions(const QSqlRecord &record) {}

bool MockQueryForm::insertRecord(QSqlRecord &record, QSqlError &error) {
    return true;
}

bool MockQueryForm::updateRecord(int row, const QSqlRecord &record,
                                 QSqlError &error) {
    return true;
}

bool MockQueryForm::removeRow(int row, QSqlError &error) {
    auto courseCode = model()->record(row).value("code").toString();
    if (manager().deleteCourse(courseCode, error)) {
        refreshModel();
        return true;
    }
    return false;
}
