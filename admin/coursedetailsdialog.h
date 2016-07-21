#ifndef COURSEDETAILSDIALOG_H
#define COURSEDETAILSDIALOG_H

#include "course.h"
#include "entity.h"
#include "entitytablemodel.h"
#include "pasodb.h"
#include "stablerownumbersortfilterproxymodel.h"
#include "student.h"
#include <memory>

#include <QDialog>

namespace Ui {
class CourseDetailsDialog;
}

namespace paso {
namespace admin {

class CourseDetailsDialog : public QDialog {
    Q_OBJECT

public:
    explicit CourseDetailsDialog(const paso::data::entity::Course &course,
                                 QWidget *parent = 0);
    ~CourseDetailsDialog();

public slots:
    virtual int exec() override;
    virtual void reject() override;
    virtual void accept() override;

private slots:
    void enlistedCountChanged(size_t count);
    void addButtonClicked();
    void removeButtonClicked();

private:
    Ui::CourseDetailsDialog *ui;

    struct CourseDetailsDialogPrivate;
    std::unique_ptr<CourseDetailsDialogPrivate> mPrivate;
};
}
}
#endif // COURSEDETAILSDIALOG_H
