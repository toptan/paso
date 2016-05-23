#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "logindialog.h"
#include "pasodb.h"

#include <QDebug>

namespace paso {
namespace admin {

using namespace Ui;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    mLoginDialog = new LoginDialog(this);
    connect(mLoginDialog, &LoginDialog::rejected, this, &MainWindow::close);
    connect(mLoginDialog, &LoginDialog::loginSuccessfull, this,
            &MainWindow::loginSuccessfull);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::show() {
    QMainWindow::show();
    mLoginDialog->show();
}

void MainWindow::loginSuccessfull(const QString &dbUsername,
                                  const QString &dbPassword,
                                  const QString &dbServer,
                                  const QString &username,
                                  data::SystemRole systemRole) {
    qDebug() << dbUsername << ":" << dbPassword << "@" << dbServer << " - "
             << username << ":" << db::roleToString(systemRole);
    createMenus(systemRole);
}

void MainWindow::createMenus(data::SystemRole systemRole) {
}

}
}
