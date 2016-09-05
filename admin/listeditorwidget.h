#ifndef LISTEDITORWIDGET_H
#define LISTEDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

class ListEditorWidget : public widget::RecordEditorWidget {
public:
    ListEditorWidget(const widget::FieldTypes &fieldTypes,
                     QWidget *parent = nullptr);

    virtual void setupUi(const QVariantMap &columnLabels,
                         const QSqlRecord &record,
                         const QStringList &filterFields = {}) override;

protected:
    virtual void prepareEdit(QSqlRecord &record) override;
    virtual bool fieldReadOnly(const QString &key) override;

    virtual QLineEdit *createLineEdit(const QString &field) override;
    virtual QCheckBox *createCheckBox(const QString &field) override;
    virtual QDateEdit *createDateEdit(const QString &field) override;
};
}
}

#endif // LISTEDITORWIDGET_H
