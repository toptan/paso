#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

#include "data.h"
#include "commdata.h"
#include "commmanager.h"

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

signals:
    void loginFinished(const comm::LoginResponse &loginResponse);

private:
    Ui::LoginDialog *ui;
    std::shared_ptr<paso::comm::CommManager> commManager;

    void performLogin();

private slots:
    void loginSuccessfull(const comm::LoginResponse &loginResponse);
    void loginFailed();
    void communicationError(const QString &reason);
};
}
}

#endif // LOGINDIALOG_H
