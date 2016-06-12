#ifndef SYSTEMUSEREDITORWIDGET_H
#define SYSTEMUSEREDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

class SystemUserEditorWidget : public RecordEditorWidget {
public:
    SystemUserEditorWidget(const QSqlRecord &record,
                           const FieldTypes &fieldTypes,
                           QWidget *parent = nullptr);

protected:
    virtual void prepareEdit(QSqlRecord &record) override;
    virtual bool fieldReadOnly(const QString &key) override;

private:
    bool mEditingRootSystemUser;
};
}
}

#endif // SYSTEMUSEREDITORWIDGET_H
