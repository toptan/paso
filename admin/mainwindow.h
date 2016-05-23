#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private:
    Ui::MainWindow *ui;
    LoginDialog *mLoginDialog;
};
}
}

#endif // MAINWINDOW_H
