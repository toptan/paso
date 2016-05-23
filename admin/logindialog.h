#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

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

private:
    Ui::LoginDialog *ui;

    void performLogin();
};
}
}

#endif // LOGINDIALOG_H
