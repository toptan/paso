#ifndef LISTDETAILSDIALOG_H
#define LISTDETAILSDIALOG_H

#include "list.h"
#include <memory>

#include <QDialog>

namespace Ui {
class ListDetailsDialog;
}

class QAbstractButton;

namespace paso {
namespace admin {

///
/// \brief The ListDetailsDialog class is a dialog that displays list details
/// and allows list changing or importing list member students.
///
class ListDetailsDialog : public QDialog {
    Q_OBJECT

public:
    ///
    /// \brief Constructs dialog for given list.
    /// \param list The list.
    /// \param parent The parent widget.
    ///
    explicit ListDetailsDialog(const paso::data::entity::List &list,
                               QWidget *parent = nullptr);
    /// Destructor.
    virtual ~ListDetailsDialog();

public slots:
    ///
    /// \brief Reimplemented QDialog::reject() slot to warn user if there is
    /// unsaved data.
    ///
    virtual void reject() override;
    ///
    /// \brief Called when user selects file to import student list.
    /// \param fileName The file to use.
    ///
    void onImportFileSelected(const QString &fileName);
    ///
    /// \brief Loads lists detailed data, also called when refresh button is
    /// clicked.
    /// \return \c true if loading succeeded.
    ///
    bool loadData();

signals:
    ///
    /// \brief This signal is emited for each imported line during student list
    /// import to update log dialog.
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

private:
    Ui::ListDetailsDialog *ui; //!< The generated form object.

    struct ListDetailsDialogPrivate;
    /// The pointer to dialogs private data.
    std::unique_ptr<ListDetailsDialogPrivate> mPrivate;

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
    /// \brief Performs actual import of the student list.
    ///
    void importListStudents();
};
}
}

#endif // LISTDETAILSDIALOG_H
