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
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    void createDbConnection(const QString &connectionName,
                            const comm::LoginResponse &response);

public slots:
    void show();
    void loginFinished(const comm::LoginResponse &response);

private slots:
    void onWidgetChooserCurrentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    LoginDialog *mLoginDialog;
    QStackedWidget *mMainWidget;
    paso::data::SystemRole mRole;
    QComboBox *mWidgetChooserComboBox;
    QAction *mWidgetChooserSeparator;
    void createMenus();
    void createWidgets();

    void setupAdministratorUI();
    void setupRoomManagerUI();
    void setupManagerUI();
};
}
}

#endif // MAINWINDOW_H
