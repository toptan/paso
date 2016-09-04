#ifndef RECORDDISPLAYWIDGET_H
#define RECORDDISPLAYWIDGET_H

#include "recordvalidator.h"

#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QMap>
#include <QSpinBox>
#include <QSqlRecord>
#include <QString>
#include <QStringList>
#include <QTimeEdit>
#include <QVariantMap>
#include <QWidget>

namespace paso {
namespace widget {

class RecordEditorWidget : public QWidget {
    Q_OBJECT
public:
    explicit RecordEditorWidget(const FieldTypes &fieldTypes,
                                QWidget *parent = nullptr);
    virtual ~RecordEditorWidget();

    virtual void setupUi(const QVariantMap &columnLabels,
                         const QSqlRecord &record);

    const FieldTypes &fieldTypes() const;
    const FieldEditors &fieldEditors() const;

    void setValidator(RecordValidator *validator);

signals:
    void editFinished();
    void requestUpdate(QSqlRecord record);
    void requestSave(QSqlRecord record);

public slots:
    virtual void onDisplayRecord(const QSqlRecord &record);
    virtual void onEditExistingRecord(QSqlRecord record);
    virtual void onEditNewRecord(QSqlRecord record);
    virtual void clearData();
    virtual void saveSuccessfull();

protected:
    virtual void prepareEdit(QSqlRecord &record) = 0;
    virtual bool fieldReadOnly(const QString &key) = 0;

    virtual QLineEdit *createLineEdit(const QString &field);
    virtual QLineEdit *createMaskedLineEdit(const QString &field);
    virtual QLineEdit *createPasswordLineEdit(const QString &field);
    virtual QComboBox *createComboBox(const QString &field);
    virtual QSpinBox *createSpinBox(const QString &field);
    virtual QCheckBox *createCheckBox(const QString &field);
    virtual QDateEdit *createDateEdit(const QString &field);
    virtual QTimeEdit *createTimeEdit(const QString &field);

    void setFieldsReadOnly();

protected slots:
    virtual void rejected();
    virtual void accepted();

protected:
    QSqlRecord mRecord;
    bool mNewRecord;

private:
    const FieldTypes mFieldTypes;
    FieldEditors mFieldEditors;
    QDialogButtonBox *mButtonBox;
    RecordValidator *mValidator;

    QWidget *createWidgetForField(const QSqlRecord &record, int index);
    void setFieldsEditable();
    void focusFirstEditable();
};
}
}

#endif // RECORDDISPLAYWIDGET_H
