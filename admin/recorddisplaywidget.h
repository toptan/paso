#ifndef RECORDDISPLAYWIDGET_H
#define RECORDDISPLAYWIDGET_H

#include "recordvalidator.h"

#include <QDialogButtonBox>
#include <QLineEdit>
#include <QMap>
#include <QSqlRecord>
#include <QString>
#include <QStringList>
#include <QWidget>

namespace paso {
namespace admin {

class RecordDisplayWidget : public QWidget {
    Q_OBJECT
public:
    explicit RecordDisplayWidget(QWidget *parent = 0);
    virtual ~RecordDisplayWidget() {}

    void setupForRecord(const QSqlRecord &record,
                        const QMap<QString, FieldType> &fieldEntryTypes);
    const FieldTypes &fieldTypes() const;
    const FieldEditors &fieldEditors() const;

    void setValidator(RecordValidator *validator);

signals:
    void editFinished();
    void requestUpdate(QSqlRecord record);
    void requestSave(QSqlRecord record);

public slots:
    void onDisplayRecord(const QSqlRecord &record);
    void onEditExistingRecord(QSqlRecord record);
    void onEditNewRecord(QSqlRecord record);
    void clearData();
    void saveSuccessfull();
    void saveError();

private:
    FieldTypes mFieldTypes;
    FieldEditors mFieldEditors;
    QSqlRecord mRecord;
    QDialogButtonBox *mButtonBox;
    RecordValidator *mValidator;
    bool mNewRecord;
    bool mEditingRootSystemUser;
    QWidget *createWidgetForField(const QSqlRecord &record, int index);
    void setFieldsEditable();
    void setFieldsReadOnly();

private slots:
    void rejected();
    void accepted();
};
}
}

#endif // RECORDDISPLAYWIDGET_H
