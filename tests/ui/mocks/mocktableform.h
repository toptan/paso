#ifndef MOCKTABLEFORM_H
#define MOCKTABLEFORM_H

#include "tableform.h"

using namespace paso::widget;

class MockTableForm : public TableForm {
public:
    MockTableForm();
    virtual ~MockTableForm();

    void setReadOnly(bool readOnly = true);

    QSqlRecord getSelectedRecord() const;

public:
    virtual void prepareRecordForSaving(QSqlRecord &record) override;

    virtual bool
    shouldEnableEditAction(const QSqlRecord &record) const override;

    virtual bool
    shouldEnableDeleteAction(const QSqlRecord &record) const override;

    virtual bool shouldDeleteRecord(const QSqlRecord &record) const override;

    virtual void updateActions(const QSqlRecord &record) override;

private:
    static std::pair<QSqlTableModel *, RecordEditorWidget *>
    createModelAndEditor();
    bool mReadOnly;
};

#endif // MOCKTABLEFORM_H
