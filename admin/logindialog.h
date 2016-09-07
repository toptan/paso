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

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);
    virtual ~LoginDialog();

public slots:
    virtual void accept() override;
    void onLoginSuccessfull(const paso::comm::LoginResponse &loginResponse);
    void onLoginFailed();
    void onCommunicationError(const QString &reason);

signals:
    void loginFinished(const paso::comm::LoginResponse loginResponse);
    void loginAttemptFailed(const QString &reason);
    void communicationError(const QString &reason);

private:
    Ui::LoginDialog *ui;

    void performLogin();
};
}
}

#endif // LOGINDIALOG_H
