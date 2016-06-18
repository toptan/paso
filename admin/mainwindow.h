#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "data.h"
#include "commdata.h"
#include "abstractform.h"

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
    QMap<paso::data::SystemRole, AbstractForm *> mMainWidgets;
    QComboBox *mWidgetChooserComboBox;
    QAction *mWidgetChooserSeparator;
    void createMenus();
    void createWidgets();
    void showWidgets(paso::data::SystemRole role);
};
}
}

#endif // MAINWINDOW_H
