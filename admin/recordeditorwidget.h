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

class RecordEditorWidget : public QWidget {
    Q_OBJECT
public:
    explicit RecordEditorWidget(const QSqlRecord &record,
                                const FieldTypes &fieldTypes,
                                QWidget *parent = 0);
    virtual ~RecordEditorWidget() {}

    void setupUi(const QSqlRecord& record);

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

protected:
    virtual void prepareEdit(QSqlRecord &record) = 0;
    virtual bool fieldReadOnly(const QString &key) = 0;

    virtual QWidget *createComboBoxForRecordField(const QString &field) = 0;

private:
    const FieldTypes mFieldTypes;
    FieldEditors mFieldEditors;
    QSqlRecord mRecord;
    QDialogButtonBox *mButtonBox;
    RecordValidator *mValidator;
    bool mNewRecord;
    QWidget *mFirstResponder;

    QWidget *createWidgetForField(const QSqlRecord &record, int index);
    void setFieldsEditable();
    void setFieldsReadOnly();
    void focusFirstEditable();

private slots:
    void rejected();
    void accepted();
};
}
}

#endif // RECORDDISPLAYWIDGET_H
