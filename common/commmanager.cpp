#include "commmanager.h"

#include <QThread>
#include <QtConcurrent>
#include <QDebug>
#include <QTcpSocket>

namespace paso {
namespace comm {

CommManager::CommManager(const QString &server, unsigned short port,
                         QObject *parent)
    : QObject(parent), mServer(server), mPort(port), mTimeout(5000) {}

void CommManager::login(const QString &username, const QString &password) {
    QtConcurrent::run([this, username, password]() {
        LoginRequest request(username, password);
        QTcpSocket socket;
        socket.connectToHost(mServer, mPort);
        if (!socket.waitForConnected(mTimeout)) {
            emit loginFailed();
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
        if (!socket.waitForBytesWritten(mTimeout)) {
            emit communicationError(socket.errorString());
            return;
        }
        if (!socket.waitForReadyRead(mTimeout)) {
            emit communicationError(socket.errorString());
            return;
        }
        quint16 blockSize;
        QDataStream in(&socket);
        in.setVersion(QDataStream::Qt_5_5);
        in >> blockSize;

        while (socket.bytesAvailable() < blockSize) {
            if (!socket.waitForReadyRead(mTimeout)) {
                emit communicationError(socket.errorString());
                return;
            }
        }

        QString response;
        in >> response;
        LoginResponse loginResponse;
        loginResponse.fromJsonString(response);
        if (loginResponse.dbServer().isEmpty()) {
            emit loginFailed();
        } else {
            emit loginSuccessfull(loginResponse);
        }
    });
}
}
}
