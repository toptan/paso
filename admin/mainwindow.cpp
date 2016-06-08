#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "logindialog.h"
#include "pasodb.h"
#include "administratorform.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QToolBar>

namespace paso {
namespace admin {

using namespace paso::comm;
using namespace paso::db;

using namespace Ui;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    mLoginDialog = new LoginDialog(this);
    connect(mLoginDialog, &LoginDialog::rejected, this, &MainWindow::close);
    connect(mLoginDialog, &LoginDialog::loginFinished, this,
            &MainWindow::loginFinished);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::show() {
    QMainWindow::show();
    mLoginDialog->show();
}

void MainWindow::loginFinished(const LoginResponse &response) {
    mRole = response.systemUser().role();
    auto db = QSqlDatabase::addDatabase("QPSQL", DEFAULT_DB_NAME);
    db.setDatabaseName(response.dbName());
    db.setHostName(response.dbServer());
    db.setUserName(response.dbUsername());
    db.setPassword(response.dbPassword());
    db.setPort(response.dbPort());
    if (!db.open()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Critical error"));
        ;
        msgBox.setText(tr("Could not establish database connection."));
        msgBox.setDetailedText(db.lastError().text());
        msgBox.exec();
        QApplication::instance()->quit();
    }
    createMenus();
    createWidgets();
    showWidgets();
}

void MainWindow::createMenus() {}

void MainWindow::createWidgets() {
    AdministratorForm *administratorForm = new AdministratorForm(this);
    mMainWidgets.insert(paso::data::SystemRole::ADMINISTRATOR,
                        administratorForm);

    auto toolBar = new QToolBar(tr("Main Toolbar"), this);
    toolBar->setObjectName("MainToolBar");
    addToolBar(toolBar);
}

void MainWindow::showWidgets() {
    QToolBar *toolBar = findChild<QToolBar *>("MainToolBar");
    switch (mRole) {
    case paso::data::SystemRole::SUPER_USER:
    case paso::data::SystemRole::ADMINISTRATOR:
        setCentralWidget(mMainWidgets[paso::data::SystemRole::ADMINISTRATOR]);
        toolBar->insertActions(
            nullptr, mMainWidgets[paso::data::SystemRole::ADMINISTRATOR]
                         ->toolBarActions());
        break;
    default:
        break;
    }
}
}
}
