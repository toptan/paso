#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

#include "data.h"

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
    void loginSuccessfull(const QString &dbUsername, const QString &dbPassword,
                          const QString &dbServer, const QString &username,
                          paso::data::SystemRole systemRole);

private:
    Ui::LoginDialog *ui;

    void performLogin();
};
}
}

#endif // LOGINDIALOG_H
