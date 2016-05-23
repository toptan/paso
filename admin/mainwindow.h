#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "data.h"

namespace Ui {
class MainWindow;
}

namespace paso {
namespace admin {

class LoginDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

public slots:
    void show();
    void loginSuccessfull(const QString &dbUsername, const QString &dbPassword,
                          const QString &dbServer, const QString &username,
                          paso::data::SystemRole systemRole);

private:
    Ui::MainWindow *ui;
    LoginDialog *mLoginDialog;

    void createMenus(paso::data::SystemRole systemRole);
};
}
}

#endif // MAINWINDOW_H
