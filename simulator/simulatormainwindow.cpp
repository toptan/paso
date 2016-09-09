#include "simulatormainwindow.h"
#include "ui_simulatormainwindow.h"

#include "commaccess.h"
#include "commping.h"
#include "commregister.h"

#include <QMessageBox>
#include <QSslSocket>

using namespace paso::comm;
using namespace std;

SimulatorMainWindow::SimulatorMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::SimulatorMainWindow), mSslServer(nullptr),
      mMode(Normal), mRegistered(false), mPort(0), mSslKey(nullptr),
      mSslCert(nullptr) {
    ui->setupUi(this);
    ui->registerButton->setEnabled(false);
    connect(ui->operationModeGroup, static_cast<void (QButtonGroup::*)(int)>(
                                        &QButtonGroup::buttonClicked),
            this, &SimulatorMainWindow::onRadioButtonClicked);
    ui->operationModeGroup->setId(ui->normalRadioButton, Normal);
    ui->operationModeGroup->setId(ui->nonCritialFailureRadioButton,
                                  NonCritical);
    ui->operationModeGroup->setId(ui->criticalFailureRadioButton, Critical);

    auto keysDir = QApplication::applicationDirPath() + "/../share/paso/";
    QFile keyFile(keysDir + "simulator.key");
    QFile certFile(keysDir + "simulator.csr");
    bool keyAndCertOk = true;
    if (!keyFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(
            this, tr("Error"),
            tr("Could not open server key %1. Simulator will not be able to "
               "listen for communication check messages.")
                .arg("/tmp/server.key"));
        keyAndCertOk = false;
    }
    if (!certFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Error"),
                             tr("Could not open server certificate %1. "
                                "Simulator will not be able to "
                                "listen for communication check messages.")
                                 .arg("/tmp/server.csr"));
        keyAndCertOk = false;
    }

    if (keyAndCertOk) {
        mSslKey = std::make_shared<QSslKey>(&keyFile, QSsl::Rsa, QSsl::Pem,
                                            QSsl::PrivateKey);
        mSslCert = std::make_shared<QSslCertificate>(&certFile);
    }
}

SimulatorMainWindow::~SimulatorMainWindow() { delete ui; }

void SimulatorMainWindow::useEmergencyData() {
    ui->sentMessagesEdit->appendPlainText(
        tr("PaSo server %1 is not responding. Using emergency data.")
            .arg(mServer));
    auto rfid = ui->cardEdit->text().trimmed();
    if (mEmergencyData.contains(rfid)) {
        ui->sentMessagesEdit->appendPlainText(
            tr("Person %1 found in emergency data. The door is unlocked.")
                .arg(rfid));
    } else {
        ui->sentMessagesEdit->appendPlainText(
            tr("Person %1 was not found in emergency data. The door will "
               "remain "
               "locked.")
                .arg(rfid));
    }
    ui->sentMessagesEdit->appendPlainText("================================");
}

void SimulatorMainWindow::onRegisterButtonClicked() {
    if (mSslServer != nullptr) {
        delete mSslServer;
        mSslServer = nullptr;
    }
    mServer = ui->serverEdit->text().trimmed();
    RegisterRequest request(ui->uuidEdit->text().trimmed());
    QSslSocket socket;
    socket.setPeerVerifyMode(QSslSocket::VerifyNone);
    socket.connectToHostEncrypted(mServer, 6789);
    if (!socket.waitForEncrypted()) {
        ui->sentMessagesEdit->appendPlainText("*** " + socket.errorString());
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
        ui->sentMessagesEdit->appendPlainText("*** " + socket.errorString());
        socket.disconnectFromHost();
        return;
    }
    ui->sentMessagesEdit->appendPlainText(request.toJsonString().trimmed());
    ui->sentMessagesEdit->appendPlainText("================================");
    if (!socket.waitForReadyRead()) {
        ui->sentMessagesEdit->appendPlainText("*** " + socket.errorString());
        socket.disconnectFromHost();
        return;
    }
    quint16 blockSize;
    QDataStream in(&socket);
    in.setVersion(QDataStream::Qt_5_5);
    in >> blockSize;

    while (socket.bytesAvailable() < blockSize) {
        if (!socket.waitForReadyRead()) {
            ui->sentMessagesEdit->appendPlainText("*** " +
                                                  socket.errorString());
            socket.disconnectFromHost();
            return;
        }
    }

    QString response;
    in >> response;
    RegisterResponse registerResponse;
    registerResponse.fromJsonString(response);
    ui->receivedMessagesEdit->appendPlainText(response.trimmed());
    ui->receivedMessagesEdit->appendPlainText(
        "================================");
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
        mRoomUUID = registerResponse.roomId();
        if (mSslKey && mSslCert) {
            mSslServer = new SslServer(mSslCert, mSslKey, this);
            connect(mSslServer, &QTcpServer::newConnection, this,
                    &SimulatorMainWindow::handlePingRequest);
            if (!mSslServer->listen(QHostAddress::Any, mPort)) {
                QMessageBox::warning(
                    this, tr("Error"),
                    tr("Cannot listen to the port %1. Simulator will not be "
                       "able to listen to communication check messages. Error "
                       "is: %2")
                        .arg(mPort)
                        .arg(mSslServer->errorString()));
                delete mSslServer;
            }
        } else {
            ui->statusLabel->setText(tr("Registered, but not listening for "
                                        "communication check messages."));
        }
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
    if (mSslServer != nullptr) {
        mSslServer->close();
        if (mMode != Critical) {
            mSslServer->listen(QHostAddress::Any, mPort);
        }
    }
}

void SimulatorMainWindow::onReadCardButtonClicked() {
    QString rfid = ui->cardEdit->text().trimmed();
    AccessRequest request(mRoomUUID, rfid);
    QSslSocket socket;
    socket.setPeerVerifyMode(QSslSocket::VerifyNone);
    socket.connectToHostEncrypted(mServer, 6789);
    if (!socket.waitForEncrypted()) {
        ui->sentMessagesEdit->appendPlainText("*** " + socket.errorString());
        socket.disconnectFromHost();
        useEmergencyData();
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
        ui->sentMessagesEdit->appendPlainText("*** " + socket.errorString());
        socket.disconnectFromHost();
        useEmergencyData();
        return;
    }
    ui->sentMessagesEdit->appendPlainText(request.toJsonString().trimmed());
    ui->sentMessagesEdit->appendPlainText("================================");
    if (!socket.waitForReadyRead()) {
        ui->sentMessagesEdit->appendPlainText("*** " + socket.errorString());
        socket.disconnectFromHost();
        useEmergencyData();
        return;
    }
    quint16 blockSize;
    QDataStream in(&socket);
    in.setVersion(QDataStream::Qt_5_5);
    in >> blockSize;

    while (socket.bytesAvailable() < blockSize) {
        if (!socket.waitForReadyRead()) {
            ui->sentMessagesEdit->appendPlainText("*** " +
                                                  socket.errorString());
            socket.disconnectFromHost();
            useEmergencyData();
            return;
        }
    }

    QString response;
    in >> response;
    AccessResponse accessResponse;
    accessResponse.fromJsonString(response);
    ui->receivedMessagesEdit->appendPlainText(response.trimmed());
    ui->receivedMessagesEdit->appendPlainText(
        "================================");
    if (accessResponse.granted()) {
        ui->cardEdit->clear();
    }
    if (accessResponse.reRegister()) {
        onRegisterButtonClicked();
    }
}

void SimulatorMainWindow::handlePingRequest() {
    ui->receivedMessagesEdit->appendPlainText(
        tr("Got ping request from the server."));
    auto serverSocket = mSslServer->nextPendingConnection();
    connect(serverSocket, &QTcpSocket::disconnected, serverSocket,
            &QObject::deleteLater);

    if (!serverSocket->waitForReadyRead()) {
        ui->receivedMessagesEdit->appendPlainText(
            tr("Failed to read server ping request: %1")
                .arg(serverSocket->errorString()));
        ui->receivedMessagesEdit->appendPlainText(
            "================================");
        serverSocket->disconnectFromHost();
        return;
    }

    quint16 blockSize;
    QDataStream in(serverSocket);
    in.setVersion(QDataStream::Qt_5_5);
    in >> blockSize;

    while (serverSocket->bytesAvailable() < blockSize) {
        if (!serverSocket->waitForReadyRead()) {
            ui->receivedMessagesEdit->appendPlainText(
                tr("The server ping request was incomplete. Error was: %1")
                    .arg(serverSocket->errorString()));
            ui->receivedMessagesEdit->appendPlainText(
                "================================");
            serverSocket->disconnectFromHost();
            return;
        }
    }
    QString requestData;
    in >> requestData;
    PingRequest request;
    request.fromJsonString(requestData);
    if (request.operation() != Operation::PING) {
        ui->receivedMessagesEdit->appendPlainText(
            tr("The server request is not ping request. Message was: %1")
                .arg(requestData));
        ui->receivedMessagesEdit->appendPlainText(
            "================================");
        serverSocket->disconnectFromHost();
        return;
    }

    ui->receivedMessagesEdit->appendPlainText(requestData);
    ui->receivedMessagesEdit->appendPlainText(
        "================================");

    PingResponse response(mRoomUUID);
    if (mMode == NonCritical) {
        response.setResponse(false);
        response.setFault("The device FOO malfunctions.");
    }

    QString responseData = response.toJsonString();
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_5);
    out << (quint16)0;
    out << responseData;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    serverSocket->write(block);
    serverSocket->disconnectFromHost();
    if (mMode == Normal) {
        ui->sentMessagesEdit->appendPlainText(
            tr("Respondend back to server that everything is OK."));
    } else {
        ui->sentMessagesEdit->appendPlainText(
            tr("Responded to the server with non critical fault '%1'")
                .arg(response.fault()));
    }
    ui->sentMessagesEdit->appendPlainText(responseData);
    ui->sentMessagesEdit->appendPlainText("================================");
}
