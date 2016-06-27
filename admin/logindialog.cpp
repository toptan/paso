#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDebug>

namespace paso {
namespace admin {

using namespace paso::data;
using namespace paso::comm;

using namespace Ui;

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginDialog) {
    ui->setupUi(this);
    setWindowFlags((windowFlags() | Qt::CustomizeWindowHint) &
                   ~Qt::WindowMinMaxButtonsHint);
    ui->usernameLineEdit->setText("root");
    ui->passwordLineEdit->setText("root");
    ui->serverLineEdit->setText("localhost");
}

LoginDialog::~LoginDialog() { delete ui; }

void LoginDialog::accept() {
    ui->messageLabel->clear();
    if (ui->usernameLineEdit->text().isEmpty()) {
        ui->messageLabel->setText(tr("You need to provide username."));
        ui->usernameLineEdit->setFocus();
        return;
    }
    if (ui->passwordLineEdit->text().isEmpty()) {
        ui->messageLabel->setText(tr("You need to provide password."));
        ui->passwordLineEdit->setFocus();
        return;
    }
    if (ui->serverLineEdit->text().isEmpty()) {
        ui->messageLabel->setText(tr("You need to provide server name."));
        ui->serverLineEdit->setFocus();
        return;
    }
    performLogin();
}

void LoginDialog::performLogin() {
    commManager = std::make_shared<CommManager>(ui->serverLineEdit->text());
    connect(commManager.get(), &CommManager::loginSuccessfull, this,
            &LoginDialog::loginSuccessfull);
    connect(commManager.get(), &CommManager::loginFailed, this,
            &LoginDialog::loginFailed);
    connect(commManager.get(), &CommManager::communicationError, this,
            &LoginDialog::communicationError);
    commManager->login(ui->usernameLineEdit->text(),
                       ui->passwordLineEdit->text());
    ui->usernameLineEdit->setEnabled(false);
    ui->passwordLineEdit->setEnabled(false);
    ui->serverLineEdit->setEnabled(false);
}

void LoginDialog::loginSuccessfull(const LoginResponse &loginResponse) {
    QDialog::accept();
    emit loginFinished(loginResponse);
}

void LoginDialog::loginFailed() {
    ui->messageLabel->setText(tr("Login failed."));
    ui->usernameLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->serverLineEdit->setEnabled(true);
}

void LoginDialog::communicationError(const QString &reason) {
    ui->messageLabel->setText(reason);
    ui->usernameLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->serverLineEdit->setEnabled(true);
}
}
}
