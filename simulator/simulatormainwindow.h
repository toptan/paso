#ifndef SIMULATORMAINWINDOW_H
#define SIMULATORMAINWINDOW_H

#include "sslserver.h"

#include <QMainWindow>
#include <QUuid>
#include <QSslKey>
#include <QSslCertificate>

#include <memory>

namespace Ui {
class SimulatorMainWindow;
}

class SimulatorMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit SimulatorMainWindow(QWidget *parent = 0);
    ~SimulatorMainWindow();

public slots:
    void onRegisterButtonClicked();
    void onClearButtonClicked();
    void onServerOrUUIDTextChanged(const QString &newText);
    void onRadioButtonClicked(int id);
    void onReadCardButtonClicked();

private:
    enum Mode { Normal = 1, NonCritical, Critical };
    Ui::SimulatorMainWindow *ui;

    paso::comm::SslServer *mSslServer;

    Mode mMode;
    bool mRegistered;
    quint16 mPort;
    QString mServer;
    QUuid mRoomUUID;
    QStringList mEmergencyData;

    std::shared_ptr<QSslKey> mSslKey;
    std::shared_ptr<QSslCertificate> mSslCert;

    void useEmergencyData();

private slots:
    void handlePingRequest();
};

#endif // SIMULATORMAINWINDOW_H
