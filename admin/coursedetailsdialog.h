#ifndef COURSEDETAILSDIALOG_H
#define COURSEDETAILSDIALOG_H

#include "course.h"
#include <memory>

#include <QDialog>

namespace Ui {
class CourseDetailsDialog;
}

class QAbstractButton;

namespace paso {
namespace admin {

class CourseDetailsDialog : public QDialog {
    Q_OBJECT

public:
    explicit CourseDetailsDialog(const paso::data::entity::Course &course,
                                 QWidget *parent = nullptr);
    virtual ~CourseDetailsDialog();

public slots:
    virtual void reject() override;
    void onImportFileSelected(const QString &fileName);

signals:
    void newLogLine(const QString &line);
    void importDone();

private slots:
    virtual void onButtonBoxButtonClicked(QAbstractButton *button);

private:
    Ui::CourseDetailsDialog *ui;

    struct CourseDetailsDialogPrivate;
    std::unique_ptr<CourseDetailsDialogPrivate> mPrivate;

    void refresh();
    bool loadData();
    bool saveData();
    void importCourseStudents();
};
}
}
#endif // COURSEDETAILSDIALOG_H
