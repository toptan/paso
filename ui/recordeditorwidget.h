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
    void onDisplayRecord(const QSqlRecord &record);
    void onEditExistingRecord(QSqlRecord record);
    void onEditNewRecord(QSqlRecord record);
    void clearData();
    void saveSuccessfull();

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

private:
    const FieldTypes mFieldTypes;
    FieldEditors mFieldEditors;
    QSqlRecord mRecord;
    QDialogButtonBox *mButtonBox;
    RecordValidator *mValidator;
    bool mNewRecord;

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
