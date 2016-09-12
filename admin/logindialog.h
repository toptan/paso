#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

#include "commdata.h"
#include "data.h"

#include <memory>

namespace Ui {
class LoginDialog;
}

namespace paso {
namespace admin {

///
/// \brief The LoginDialog class represents user login form.
///
class LoginDialog : public QDialog {
    Q_OBJECT

public:
    ///
    /// \brief Contructs login dialog.
    /// \param parent The parent widget.
    ///
    explicit LoginDialog(QWidget *parent = 0);

    ///
    /// \brief Desctructor.
    ///
    virtual ~LoginDialog();

public slots:
    ///
    /// \brief accept Overriden accept slot that starts login process.
    ///
    virtual void accept() override;

    ///
    /// \brief This slot is called when login is successful.
    /// \param loginResponse The login response.
    ///
    void onLoginSuccessfull(const paso::comm::LoginResponse &loginResponse);
    ///
    /// \brief This slot is called if login fails.
    ///
    void onLoginFailed();
    ///
    /// \brief This slog gets called whenever there is communication error.
    /// \param reason The reason for the error.
    ///
    void onCommunicationError(const QString &reason);

signals:
    ///
    /// \brief This signal is emitted when login process is finished.
    /// \param loginResponse The received login response.
    ///
    void loginFinished(const paso::comm::LoginResponse loginResponse);
    ///
    /// \brief This signal is emited when login attempt fails.
    /// \param reason The reason of the failure.
    ///
    void loginAttemptFailed(const QString &reason);
    ///
    /// \brief This signal is emited when there is a communication failure.
    /// \param reason The reason of the failure.
    ///
    void communicationError(const QString &reason);

private:
    Ui::LoginDialog *ui; //!< The generated form object.

    ///
    /// \brief Starts actual login process with the server.
    ///
    void performLogin();
};
}
}

#endif // LOGINDIALOG_H
