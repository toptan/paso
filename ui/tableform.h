#ifndef TABLEFORM_H
#define TABLEFORM_H

#include "abstractform.h"

#include <utility>

class QSqlTableModel;

namespace paso {
namespace widget {

///
/// \brief The TableForm class is an AbstractForm tailored to work with forms
/// attached to database tables.
///
class TableForm : public AbstractForm {
    Q_OBJECT
public:
    ///
    /// \brief Constructs new table form with given model and editor.
    /// \param modelAndEditor The table model and editor to use.
    /// \param parent The parent widget.
    ///
    explicit TableForm(
        std::pair<QSqlTableModel *, RecordEditorWidget *> modelAndEditor,
        QWidget *parent = nullptr);

    ///
    /// \brief Destructor.
    ///
    virtual ~TableForm();

protected:
    /// \copydoc AbstractForm::removeRow()
    virtual bool removeRow(int row, QSqlError &error) override;
    /// \copydoc AbstractForm::insertRecord()
    virtual bool insertRecord(QSqlRecord &record, QSqlError &error) override;
    /// \copydoc AbstractForm::updateRecord()
    virtual bool updateRecord(int row, const QSqlRecord &record,
                              QSqlError &error) override;

private:
    ///
    /// \brief A helper method that casts \ref AbstractForm::model() to SQL
    /// table model.
    /// \return The model that the form uses.
    ///
    QSqlTableModel *tableModel() const;
};
}
}

#endif // TABLEFORM_H
