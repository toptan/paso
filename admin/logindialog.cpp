#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDebug>

namespace paso {
namespace admin {

using namespace Ui;

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog) {
    ui->setupUi(this);
}

LoginDialog::~LoginDialog() { delete ui; }

void LoginDialog::accept() {
    ui->messageLabel->clear();
    if(ui->usernameLineEdit->text().isEmpty()) {
        ui->messageLabel->setText(tr("You need to provide username."));
        ui->usernameLineEdit->setFocus();
        return;
    }
    if(ui->passwordLineEdit->text().isEmpty()) {
        ui->messageLabel->setText(tr("You need to provide password."));
        ui->passwordLineEdit->setFocus();
        return;
    }
    if(ui->serverLineEdit->text().isEmpty()) {
        ui->messageLabel->setText(tr("You need to provide server name."));
        ui->serverLineEdit->setFocus();
        return;
    }
    performLogin();
}

void LoginDialog::performLogin() {

}
}
}
