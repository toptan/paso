#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "abstractform.h"
#include "commdata.h"
#include "data.h"

#include <QMainWindow>
#include <QMap>

class QAction;
class QComboBox;
class QStackedWidget;

namespace Ui {
class MainWindow;
}

namespace paso {
namespace admin {

class LoginDialog;

///
/// \brief The MainWindow class is the main window form class of the
/// admistration application.
///
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    ///
    /// \brief Constructs main window with given parent.
    /// \param parent The parent widget.
    ///
    explicit MainWindow(QWidget *parent = 0);
    ///
    /// \brief Destructor.
    ///
    virtual ~MainWindow();

    ///
    /// \brief Tries to connect to the database with the data received in login
    /// response. If connecting fails the error message box will be shown and
    /// the aplication will exit.
    /// \param connectionName The internal name of connection to use.
    /// \param response The received login response.
    ///
    void createDbConnection(const QString &connectionName,
                            const comm::LoginResponse &response);

public slots:
    ///
    /// \brief Overrides default behavior to show login dialog as soon as this
    /// window is shown.
    ///
    void show();
    ///
    /// \brief This slot is called when login is finished to receive login
    /// response.
    /// \param response The received login response.
    ///
    void loginFinished(const comm::LoginResponse &response);

private slots:
    ///
    /// \brief This slot is called whenever selection in the module combo
    /// changes to display selected module and its actions in the toolbar.
    /// \param index Index of new selection.
    ///
    void onWidgetChooserCurrentIndexChanged(int index);

private:
    Ui::MainWindow *ui;           //!< The generated form object.
    LoginDialog *mLoginDialog;    //!< The login dialog to display on start.
    QStackedWidget *mMainWidget;  //!< The windows main widget.
    paso::data::SystemRole mRole; //!< The system role of logged user.

    /// The submodule chooser combo.
    QComboBox *mWidgetChooserComboBox;
    /// The visual toolbar separator in front of the submodule chooser.
    QAction *mWidgetChooserSeparator;

    ///
    /// \brief Creates all necessary widgets and form for this window with
    /// respect to the logged user's system role.
    ///
    void createWidgets();

    ///
    /// \brief Sets up window UI for administrator role.
    ///
    void setupAdministratorUI();
    ///
    /// \brief Sets up window UI for manager role.
    ///
    void setupManagerUI();
};
}
}

#endif // MAINWINDOW_H
