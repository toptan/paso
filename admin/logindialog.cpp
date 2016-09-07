#include "logindialog.h"
#include "ui_logindialog.h"

#include <QDebug>
#include <QPushButton>
#include <QSslSocket>
#include <QtConcurrent>

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
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
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
    connect(this, &LoginDialog::communicationError, this,
            &LoginDialog::onCommunicationError);

    QtConcurrent::run([this]() {
        LoginRequest request(ui->usernameLineEdit->text(),
                             ui->passwordLineEdit->text());
        QSslSocket socket;
        socket.setPeerVerifyMode(QSslSocket::VerifyNone);
        socket.connectToHostEncrypted(ui->serverLineEdit->text(), 6789);
        if (!socket.waitForEncrypted()) {
            qWarning() << socket.errorString();
            emit communicationError(socket.errorString());
            return;
        }
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_5);
        out << (quint16)0;
        out << request.toJsonString();
        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));
        socket.write(block);
        if (!socket.waitForBytesWritten()) {
            emit communicationError(socket.errorString());
            return;
        }
        if (!socket.waitForReadyRead()) {
            emit communicationError(socket.errorString());
            return;
        }
        quint16 blockSize;
        QDataStream in(&socket);
        in.setVersion(QDataStream::Qt_5_5);
        in >> blockSize;

        while (socket.bytesAvailable() < blockSize) {
            if (!socket.waitForReadyRead()) {
                emit communicationError(socket.errorString());
                return;
            }
        }

        QString response;
        in >> response;
        LoginResponse loginResponse;
        loginResponse.fromJsonString(response);
        if (loginResponse.dbServer().isEmpty()) {
            onLoginFailed();
        } else {
            onLoginSuccessfull(loginResponse);
        }

    });

    ui->usernameLineEdit->setEnabled(false);
    ui->passwordLineEdit->setEnabled(false);
    ui->serverLineEdit->setEnabled(false);
}

void LoginDialog::onLoginSuccessfull(const LoginResponse &loginResponse) {
    QDialog::accept();
    emit loginFinished(loginResponse);
}

void LoginDialog::onLoginFailed() {
    ui->messageLabel->setText(tr("Login failed."));
    ui->usernameLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->serverLineEdit->setEnabled(true);
    emit loginAttemptFailed(tr("Login failed."));
}

void LoginDialog::onCommunicationError(const QString &reason) {
    ui->messageLabel->setText(reason);
    ui->usernameLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->serverLineEdit->setEnabled(true);
    emit loginAttemptFailed(reason);
}
}
}
