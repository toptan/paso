#ifndef STUDENTDETAILSDIALOG_H
#define STUDENTDETAILSDIALOG_H

#include "student.h"
#include <QDialog>

namespace Ui {
class StudentDetailsDialog;
}

namespace paso {
namespace model {
class EntityTableModel;
class StableRowNumberSortFilterProxyModel;
}
}
class QAbstractButton;

namespace paso {
namespace admin {

///
/// \brief The StudentDetailsDialog class is a dialog that displays detailed
/// students data.
///
class StudentDetailsDialog : public QDialog {
    Q_OBJECT

public:
    ///
    /// \brief Constructs dialog for given student.
    /// \param student The student.
    /// \param parent The parent widget.
    ///
    explicit StudentDetailsDialog(const paso::data::entity::Student &student,
                                  QWidget *parent = 0);
    ///
    /// \brief Destructor.
    ///
    virtual ~StudentDetailsDialog();

private slots:
    ///
    /// \brief Loads student's detailed data, also called when refresh button is
    /// clicked.
    ///
    void loadData();

private:
    Ui::StudentDetailsDialog *ui; //!< The generated form object.

    /// The model that holds data about student's courses.
    paso::model::EntityTableModel *mCoursesModel;
    /// The model that holds data about student's lists.
    paso::model::EntityTableModel *mListsModel;
    /// The proxy model for courses data to enable sorting.
    paso::model::StableRowNumberSortFilterProxyModel *mCoursesProxyModel;
    /// The proxy model for lists data to enable sorting.
    paso::model::StableRowNumberSortFilterProxyModel *mListsProxyModel;

    struct StudentDetailsDialogPrivate;
    /// The pointer to dialogs private data.
    std::unique_ptr<StudentDetailsDialogPrivate> mPrivate;
};
}
}
#endif // STUDENTDETAILSDIALOG_H
