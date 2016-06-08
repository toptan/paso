#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "data.h"
#include "commdata.h"
#include "abstractform.h"

#include <QMainWindow>
#include <QMap>

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
    void loginFinished(const comm::LoginResponse &response);

private:
    Ui::MainWindow *ui;
    LoginDialog *mLoginDialog;
    paso::data::SystemRole mRole;
    QMap<paso::data::SystemRole, AbstractForm *> mMainWidgets;
    void createMenus();
    void createWidgets();
    void showWidgets();
};
}
}

#endif // MAINWINDOW_H
