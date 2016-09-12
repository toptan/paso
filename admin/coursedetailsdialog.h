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

///
/// \brief The CourseDetailsDialog class is a dialog that displays course
/// details, allows changing or importing of enlisted students.
///
class CourseDetailsDialog : public QDialog {
    Q_OBJECT

public:
    ///
    /// \brief Constructs dialog for given course.
    /// \param course The course.
    /// \param parent The parent widget.
    ///
    explicit CourseDetailsDialog(const paso::data::entity::Course &course,
                                 QWidget *parent = nullptr);
    /// Destructor.
    virtual ~CourseDetailsDialog();

public slots:
    ///
    /// \brief Reimplemented QDialog::reject() slot to warn user if there is
    /// unsaved data.
    ///
    virtual void reject() override;
    ///
    /// \brief Called when user selects file to import course students.
    /// \param fileName The file to use.
    ///
    void onImportFileSelected(const QString &fileName);

signals:
    ///
    /// \brief This signal is emited for each imported line during course
    /// students import to update log dialog.
    /// \param line The log line to append.
    ///
    void newLogLine(const QString &line);
    ///
    /// \brief This signal is emited when import is done.
    ///
    void importDone();
    ///
    /// \brief This signal is emited when import has failed.
    ///
    void importFailed();

private slots:
    ///
    /// \brief This slot handles button clicks from the button box.
    /// \param button The button that was clicked.
    ///
    virtual void onButtonBoxButtonClicked(QAbstractButton *button);
    ///
    /// \brief Loads course detailed data, also called when refresh button is
    /// clicked.
    /// \return \c true if loading succeeded.
    ///
    bool loadData();

private:
    Ui::CourseDetailsDialog *ui; //!< The generated form object.

    struct CourseDetailsDialogPrivate;
    /// The pointer to dialogs private data.
    std::unique_ptr<CourseDetailsDialogPrivate> mPrivate;

    ///
    /// \brief Refreshes data.
    ///
    void refresh();
    ///
    /// \brief Saves changes.
    /// \return \c true if save was successful.
    ///
    bool saveData();
    ///
    /// \brief Performs actual import of course students.
    ///
    void importCourseStudents();
};
}
}
#endif // COURSEDETAILSDIALOG_H
