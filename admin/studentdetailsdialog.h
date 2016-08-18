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

class StudentDetailsDialog : public QDialog {
    Q_OBJECT

public:
    explicit StudentDetailsDialog(const paso::data::entity::Student &student,
                                  QWidget *parent = 0);
    virtual ~StudentDetailsDialog();

private slots:
    void loadData();

private:
    Ui::StudentDetailsDialog *ui;
    paso::model::EntityTableModel *mCoursesModel;
    paso::model::EntityTableModel *mListsModel;
    paso::model::StableRowNumberSortFilterProxyModel *mCoursesProxyModel;
    paso::model::StableRowNumberSortFilterProxyModel *mListsProxyModel;

    struct StudentDetailsDialogPrivate;
    std::unique_ptr<StudentDetailsDialogPrivate> mPrivate;
};
}
}
#endif // STUDENTDETAILSDIALOG_H
