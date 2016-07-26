#ifndef SYSTEMUSEREDITORWIDGET_H
#define SYSTEMUSEREDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

class SystemUserEditorWidget : public widget::RecordEditorWidget {
public:
    SystemUserEditorWidget(const widget::FieldTypes &fieldTypes,
                           QWidget *parent = nullptr);

protected:
    virtual void prepareEdit(QSqlRecord &record) override;
    virtual bool fieldReadOnly(const QString &key) override;

    virtual QLineEdit *createLineEdit(const QString &field) override;
    virtual QLineEdit *createPasswordLineEdit(const QString &field) override;
    virtual QComboBox *createComboBox(const QString &field) override;

private:
    bool mEditingRootSystemUser;
};
}
}

#endif // SYSTEMUSEREDITORWIDGET_H
