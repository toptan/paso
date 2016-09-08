#include "simulatormainwindow.h"
#include "ui_simulatormainwindow.h"

#include "commregister.h"

#include <QSslSocket>

using namespace paso::comm;

SimulatorMainWindow::SimulatorMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::SimulatorMainWindow), mMode(Normal),
      mRegistered(false), mPort(0) {
    ui->setupUi(this);
    ui->registerButton->setEnabled(false);
    connect(ui->operationModeGroup, static_cast<void (QButtonGroup::*)(int)>(
                                        &QButtonGroup::buttonClicked),
            this, &SimulatorMainWindow::onRadioButtonClicked);
    ui->operationModeGroup->setId(ui->normalRadioButton, Normal);
    ui->operationModeGroup->setId(ui->nonCritialFailureRadioButton,
                                  NonCritical);
    ui->operationModeGroup->setId(ui->criticalFailureRadioButton, Critical);
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
    if (registerResponse.success()) {
        mRegistered = true;
        mPort = registerResponse.port();
        mEmergencyData = registerResponse.emergencyData();
        ui->registerButton->setEnabled(false);
        ui->serverEdit->setEnabled(false);
        ui->uuidEdit->setEnabled(false);
        ui->statusLabel->setText(tr("Registered and listening on port %1")
                                     .arg(registerResponse.port()));
        ui->serverLabel->setText(tr("Running"));
        ui->cardEdit->setEnabled(true);
        ui->normalRadioButton->setEnabled(true);
        ui->nonCritialFailureRadioButton->setEnabled(true);
        ui->criticalFailureRadioButton->setEnabled(true);
        ui->readCardButton->setEnabled(true);
        ui->cardEdit->setFocus();
    }
}

void SimulatorMainWindow::onServerOrUUIDTextChanged(const QString &) {
    ui->registerButton->setEnabled(!ui->serverEdit->text().isEmpty() &&
                                   !ui->uuidEdit->text().isEmpty());
}

void SimulatorMainWindow::onClearButtonClicked() {
    ui->sentMessagesEdit->clear();
    ui->receivedMessagesEdit->clear();
}

void SimulatorMainWindow::onRadioButtonClicked(int id) {
    mMode = Mode(id);
}

void SimulatorMainWindow::onReadCardButtonClicked() {

}
