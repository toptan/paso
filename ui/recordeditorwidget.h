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

///
/// \brief The RecordEditorWidget class is a base abstract class for all SQL
/// record editors.
///
class RecordEditorWidget : public QWidget {
    Q_OBJECT
public:
    ///
    /// \brief Constructs new record editor for given field types.
    /// \param fieldTypes The record field types.
    /// \param parent The parent widget.
    ///
    explicit RecordEditorWidget(const FieldTypes &fieldTypes,
                                QWidget *parent = nullptr);
    ///
    /// \brief Destructor.
    ///
    virtual ~RecordEditorWidget();

    ///
    /// \brief Sets up editors UI for given SQL record.
    /// \param columnLabels The labels to use for editors.
    /// \param record The SQL record.
    /// \param filterFields The list of record fields to skip.
    ///
    virtual void setupUi(const QVariantMap &columnLabels,
                         const QSqlRecord &record,
                         const QStringList &filterFields = {});

    ///
    /// \brief Returns the list of record field types.
    /// \return List of record field types.
    ///
    const FieldTypes &fieldTypes() const;

    ///
    /// \brief The list of record field editors.
    /// \return The list of record field editors.
    ///
    const FieldEditors &fieldEditors() const;

    ///
    /// \brief Attaches given record validator to this editor.
    /// \param validator The validator to attach.
    ///
    void setValidator(RecordValidator *validator);

signals:
    ///
    /// \brief This signal is emited when editing has finished.
    ///
    void editFinished();

    ///
    /// \brief This signal is emited when record update is needed efter editing.
    /// \param record The updated record.
    ///
    void requestUpdate(QSqlRecord record);

    ///
    /// \brief This signal is emited when record inserting is needed after
    /// editing.
    /// \param record The record to insert.
    ///
    void requestSave(QSqlRecord record);

public slots:
    ///
    /// \brief This slot is called whenever UI needs to display new record.
    /// \param record The record to display.
    ///
    virtual void onDisplayRecord(const QSqlRecord &record);

    ///
    /// \brief Called when editing of an existing record is requested.
    /// \param record The record to edit.
    ///
    virtual void onEditExistingRecord(QSqlRecord record);

    ///
    /// \brief Called to edit new record from scratch.
    /// \param record The new empty record.
    ///
    virtual void onEditNewRecord(QSqlRecord record);

    ///
    /// \brief Clears displayed data.
    ///
    virtual void clearData();

    ///
    /// \brief This slot is called to notify widget that record was successfully
    /// saved.
    ///
    virtual void saveSuccessfull();

protected:
    ///
    /// \brief This method is used to prepare the record for editing if that is
    /// neccessary.
    /// \param record The record to prepare for editing.
    ///
    virtual void prepareEdit(QSqlRecord &record) = 0;
    ///
    /// \brief This method is used to distinguish whether some field should be
    /// read only or not.
    /// \param key The field key.
    /// \return \c true if field should not be editable.
    ///
    virtual bool fieldReadOnly(const QString &key) = 0;

    ///
    /// \brief Creates new plain line editor.
    /// \param field The field for which returned editor will be used.
    /// \return The editor widget.
    ///
    virtual QLineEdit *createLineEdit(const QString &field);
    ///
    /// \brief Creates new masked line editor.
    /// \param field The field for which returned editor will be used.
    /// \return The editor widget.
    ///
    virtual QLineEdit *createMaskedLineEdit(const QString &field);
    ///
    /// \brief Creates new password line editor.
    /// \param field The field for which returned editor will be used.
    /// \return The editor widget.
    ///
    virtual QLineEdit *createPasswordLineEdit(const QString &field);
    ///
    /// \brief Creates new combo box to be used as editor.
    /// \param field The field for which returned editor will be used.
    /// \return The editor widget.
    ///
    virtual QComboBox *createComboBox(const QString &field);
    ///
    /// \brief Creates new spinner to be used as editor.
    /// \param field The field for which returned editor will be used.
    /// \return The editor widget.
    ///
    virtual QSpinBox *createSpinBox(const QString &field);
    ///
    /// \brief Creates new check box to be used as editor.
    /// \param field The field for which returned editor will be used.
    /// \return The editor widget.
    ///
    virtual QCheckBox *createCheckBox(const QString &field);
    ///
    /// \brief Creates new date picker to be used as editor.
    /// \param field The field for which returned editor will be used.
    /// \return The editor widget.
    ///
    virtual QDateEdit *createDateEdit(const QString &field);
    ///
    /// \brief Creates new time picker to be used as editor.
    /// \param field The field for which returned editor will be used.
    /// \return The editor widget.
    ///
    virtual QTimeEdit *createTimeEdit(const QString &field);

    ///
    /// \brief Puts all fields in read only mode.
    ///
    void setFieldsReadOnly();

    ///
    /// \brief Returns record validator that is attached to this editor.
    /// \return The record validator.
    ///
    RecordValidator *validator() const;

protected slots:
    ///
    /// \brief This slot is called when record changes are rejected by the user.
    ///
    virtual void rejected();
    ///
    /// \brief This slot is called when record changes are accepted and need to
    /// be saved.
    ///
    virtual void accepted();

protected:
    QSqlRecord mRecord;        //!< The SQL record being edited.
    bool mNewRecord;           //!< Whether editing new or updating old record.
    QStringList mFilterFields; //!< The fields to be skipped in editor.

private:
    const FieldTypes mFieldTypes; //!< The record field types.
    FieldEditors mFieldEditors;   //!< The record field editors.
    QDialogButtonBox *mButtonBox; //!< The button box.
    RecordValidator *mValidator;  //!< The record validator.

    ///
    /// \brief Creates editor widget for given record field.
    /// \param record The SQL record.
    /// \param index Index of the field to create editor for.
    /// \return The editor for the field.
    ///
    QWidget *createWidgetForField(const QSqlRecord &record, int index);

    ///
    /// \brief Marks all fields as editable by respecting value returned by \ref
    /// fieldReadOnly()
    ///
    void setFieldsEditable();

    ///
    /// \brief Gives focus to the first editable field.
    ///
    void focusFirstEditable();
};
}
}

#endif // RECORDDISPLAYWIDGET_H
