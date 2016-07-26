#ifndef LISTEDITORWIDGET_H
#define LISTEDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

class ListEditorWidget : public widget::RecordEditorWidget {
public:
    ListEditorWidget(const widget::FieldTypes &fieldTypes,
                     QWidget *parent = nullptr);

protected:
    virtual void prepareEdit(QSqlRecord &record) override;
    virtual bool fieldReadOnly(const QString &key) override;

    virtual QLineEdit *createLineEdit(const QString &field) override;
};
}
}

#endif // LISTEDITORWIDGET_H
