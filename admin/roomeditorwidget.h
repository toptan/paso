#ifndef ROOMEDITORWIDGET_H
#define ROOMEDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

class RoomEditorWidget : public RecordEditorWidget {
public:
    RoomEditorWidget(const QSqlRecord &record, const FieldTypes &fieldTypes,
                     QWidget *parent = nullptr);

protected:
    virtual void prepareEdit(QSqlRecord &record) override;
    virtual bool fieldReadOnly(const QString &key) override;
};
}
}

#endif // ROOMEDITORWIDGET_H
