#ifndef RECORDDISPLAYWIDGET_H
#define RECORDDISPLAYWIDGET_H

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
    ///
    /// \brief The FieldType enum defines widget to be used for record fields.
    ///
    enum class FieldType { LineEdit, PasswordEdit, ComboBox };

    explicit RecordDisplayWidget(QWidget *parent = 0);
    virtual ~RecordDisplayWidget() {}

    void setupForRecord(const QSqlRecord &record,
                        const QMap<QString, FieldType> &fieldEntryTypes);
signals:
    void editFinished();
    void requestSave(QSqlRecord record);

public slots:
    void onDisplayRecord(const QSqlRecord &record);
    void onEditRecord(QSqlRecord record);
    void clearData();
    void saveSuccessfull();
    void saveError();

private:
    QMap<QString, FieldType> mFieldEntryTypes;
    QSqlRecord mRecord;
    QMap<QString, QWidget *> mEditFields;
    QDialogButtonBox *mButtonBox;

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
