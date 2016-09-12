#ifndef QUERYFORM_H
#define QUERYFORM_H

#include "abstractform.h"
#include "pasodb.h"

#include <utility>

class QSqlQueryModel;

namespace paso {
namespace widget {

///
/// \brief The QueryForm class is an AbstractForm tailored to work with forms
/// attached to the SQL queries.
///
class QueryForm : public AbstractForm {
public:
    ///
    /// \brief Constructs new query form with given model and editor.
    /// \param modelAndEditor The query model and editor to use.
    /// \param parent The parent widget.
    ///
    explicit QueryForm(
        std::pair<QSqlQueryModel *, RecordEditorWidget *> modelAndEditor,
        QWidget *parent = nullptr);
    ///
    /// \brief Destructor.
    ///
    virtual ~QueryForm();
};
}
}
#endif // QUERYFORM_H
