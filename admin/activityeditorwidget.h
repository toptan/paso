#ifndef ACTIVITYEDITORWIDGET_H
#define ACTIVITYEDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

class ActivityEditorWidget : public widget::RecordEditorWidget {
    Q_OBJECT
public:
    ActivityEditorWidget(const widget::FieldTypes &fieldTypes,
                         QWidget *parent = nullptr);

    virtual void setupUi(const QVariantMap &columnLabels,
                         const QSqlRecord &record) override;

public slots:
    virtual void onDisplayRecord(const QSqlRecord &record) override;
    virtual void onEditExistingRecord(QSqlRecord record) override;
    virtual void onEditNewRecord(QSqlRecord record) override;

protected slots:
    virtual void accepted() override;

protected:
    virtual void prepareEdit(QSqlRecord &record) override;
    virtual bool fieldReadOnly(const QString &key) override;

    virtual QComboBox *createComboBox(const QString &field) override;

    QString generateRepetitionString(const QString &repetitionType,
                                     const QString &strDays);
    QString translateScheduleType(const QString &strScheduleType);
};
}
}

#endif // ACTIVITYEDITORWIDGET_H
