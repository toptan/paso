#ifndef ROOMEDITORWIDGET_H
#define ROOMEDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

class RoomEditorWidget : public widget::RecordEditorWidget {
public:
    RoomEditorWidget(const widget::FieldTypes &fieldTypes,
                     QWidget *parent = nullptr);

public slots:
    virtual void saveError() override;

protected:
    virtual void prepareEdit(QSqlRecord &record) override;
    virtual bool fieldReadOnly(const QString &key) override;

    virtual QLineEdit *createLineEdit(const QString &field) override;
};
}
}

#endif // ROOMEDITORWIDGET_H
