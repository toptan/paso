#ifndef MOCKQUERYFORM_H
#define MOCKQUERYFORM_H

#include "queryform.h"

#include <QSqlQueryModel>

using namespace paso::widget;

class MockQueryForm : public QueryForm {
public:
    MockQueryForm();
    virtual ~MockQueryForm();

protected:
    virtual void prepareRecordForSaving(QSqlRecord &record) override;

    virtual bool
    shouldEnableEditAction(const QSqlRecord &record) const override;

    virtual bool
    shouldEnableDeleteAction(const QSqlRecord &record) const override;

    virtual bool shouldDeleteRecord(const QSqlRecord &record) const override;

    virtual void updateActions(const QSqlRecord &record) override;

    virtual bool insertRecord(QSqlRecord &record, QSqlError &error) override;
    virtual bool updateRecord(int row, const QSqlRecord &record,
                              QSqlError &error) override;

    virtual bool removeRow(int row, QSqlError &error) override;

private:
    static std::pair<QSqlQueryModel *, RecordEditorWidget *>
    createModelAndEditor();
};

#endif // MOCKQUERYFORM_H
