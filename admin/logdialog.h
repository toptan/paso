#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>

namespace Ui {
class LogDialog;
}

namespace paso {
namespace admin {

///
/// \brief The LogDialog class is a dialog that is used to display output of the
/// background operations like data import.
///
class LogDialog : public QDialog {
    Q_OBJECT

public:
    ///
    /// \brief Contructs log dialog and sets given title.
    /// \param title The dialog title.
    /// \param parent The parent widget.
    ///
    explicit LogDialog(const QString &title, QWidget *parent = nullptr);

    ///
    /// \brief Destructor.
    ///
    virtual ~LogDialog();

public slots:
    ///
    /// \brief Appends a line of text to the messages display.
    /// \param line A line to append.
    ///
    void appendLine(const QString &line);
    ///
    /// \brief Called when processing is done to enable closing.
    ///
    void processingDone();
    ///
    /// \brief Overriden slot that prevents closing while operation is still in
    /// progress.
    ///
    virtual void reject() override;

private:
    Ui::LogDialog *ui;    //!< The generated form object.
    bool mWorkInProgress; //!< \c true if operation is not yet finished.
};
}
}
#endif // LOGDIALOG_H
