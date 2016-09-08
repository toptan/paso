#include "simulatormainwindow.h"
#include "ui_simulatormainwindow.h"

#include "commregister.h"

#include <QSslSocket>

using namespace paso::comm;

SimulatorMainWindow::SimulatorMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::SimulatorMainWindow), mRegistered(false) {
    ui->setupUi(this);
    ui->registerButton->setEnabled(false);
}

SimulatorMainWindow::~SimulatorMainWindow() { delete ui; }

void SimulatorMainWindow::onRegisterButtonClicked() {
    mServer = ui->serverEdit->text().trimmed();
    RegisterRequest request(ui->uuidEdit->text().trimmed());
    QSslSocket socket;
    socket.setPeerVerifyMode(QSslSocket::VerifyNone);
    socket.connectToHostEncrypted(mServer, 6789);
    if (!socket.waitForEncrypted()) {
        qWarning() << socket.errorString();
        socket.disconnectFromHost();
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
        qWarning() << socket.errorString();
        socket.disconnectFromHost();
        return;
    }
    ui->sentMessagesEdit->appendPlainText(request.toJsonString());
    ui->sentMessagesEdit->appendPlainText("\n================================");
    if (!socket.waitForReadyRead()) {
        qWarning() << socket.errorString();
        socket.disconnectFromHost();
        return;
    }
    quint16 blockSize;
    QDataStream in(&socket);
    in.setVersion(QDataStream::Qt_5_5);
    in >> blockSize;

    while (socket.bytesAvailable() < blockSize) {
        if (!socket.waitForReadyRead()) {
            qWarning() << socket.errorString();
            socket.disconnectFromHost();
            return;
        }
    }

    QString response;
    in >> response;
    RegisterResponse registerResponse;
    registerResponse.fromJsonString(response);
    ui->receivedMessagesEdit->appendPlainText(response);
    ui->receivedMessagesEdit->appendPlainText(
        "\n================================");
}

void SimulatorMainWindow::onServerOrUUIDTextChanged(const QString &) {
    ui->registerButton->setEnabled(!ui->serverEdit->text().isEmpty() &&
                                   !ui->uuidEdit->text().isEmpty());
}
