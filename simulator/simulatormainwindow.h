#ifndef SIMULATORMAINWINDOW_H
#define SIMULATORMAINWINDOW_H

#include "sslserver.h"

#include <QMainWindow>
#include <QSslCertificate>
#include <QSslKey>
#include <QUuid>

#include <memory>

namespace Ui {
class SimulatorMainWindow;
}

///
/// \brief The SimulatorMainWindow class is a simulator's main window.
///
class SimulatorMainWindow : public QMainWindow {
    Q_OBJECT

public:
    ///
    /// \brief Constructs main window with given parent.
    /// \param parent The parend widget.
    ///
    explicit SimulatorMainWindow(QWidget *parent = 0);
    ///
    /// \brief Destructor.
    ///
    virtual ~SimulatorMainWindow();

public slots:
    ///
    /// \brief Called by clicking on register button to start registration with
    /// the server.
    ///
    void onRegisterButtonClicked();
    ///
    /// \brief Clears messages.
    ///
    void onClearButtonClicked();
    ///
    /// \brief This slot is called whenever server or uuid entry text changes to
    /// enable or disable register button.
    /// \param newText
    ///
    void onServerOrUUIDTextChanged(const QString &newText);
    ///
    /// \brief This slot is executed when one of the radio buttons is clicked to
    /// change simulator's mode of operation.
    /// \param id The id of the radio button.
    ///
    void onRadioButtonClicked(int id);
    ///
    /// \brief Starts access query with the server.
    ///
    void onReadCardButtonClicked();

private:
    ///
    /// \brief The Mode enum defines possible modes of operation.
    ///
    enum Mode {
        Normal = 1,  //!< Normal operation
        NonCritical, //!< Simulate non critical error
        Critical     //!< Simulate crtical error - stop responding
    };

    Ui::SimulatorMainWindow *ui; //!< The generated form object.

    /// The SSL server to listen for comm check messages.
    paso::comm::SslServer *mSslServer;
    Mode mMode;                 //!< The mode of operation.
    bool mRegistered;           //!< \c true if simulator is registered.
    quint16 mPort;              //!< Port that simulator is listening to.
    QString mServer;            //!< The address or host name of the server.
    QUuid mRoomUUID;            //!< The simulated room UUID.
    QStringList mEmergencyData; //!< The emergency data.

    std::shared_ptr<QSslKey> mSslKey;          //!< The SSL key.
    std::shared_ptr<QSslCertificate> mSslCert; //!< The SSL certificate.

    ///
    /// \brief This method is used to simulate emergency operation when the
    /// server is down.
    ///
    void useEmergencyData();

private slots:
    ///
    /// \brief This slot is called on every incoming server's comm check
    /// message.
    ///
    void handlePingRequest();
};

#endif // SIMULATORMAINWINDOW_H
