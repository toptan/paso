#ifndef ACTIVITYEDITORWIDGET_H
#define ACTIVITYEDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

class ActivityEditorWidget : public widget::RecordEditorWidget {
public:
    ActivityEditorWidget(const widget::FieldTypes &fieldTypes,
                         QWidget *parent = nullptr);

    virtual void setupUi(const QVariantMap &columnLabels,
                         const QSqlRecord &record) override;

public slots:
    virtual void onEditExistingRecord(QSqlRecord record) override;
    virtual void onEditNewRecord(QSqlRecord record) override;

protected:
    virtual void prepareEdit(QSqlRecord &record) override;
    virtual bool fieldReadOnly(const QString &key) override;

    virtual QComboBox *createComboBox(const QString &field) override;
};
}
}

#endif // ACTIVITYEDITORWIDGET_H
