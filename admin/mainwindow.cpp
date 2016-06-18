#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "logindialog.h"
#include "pasodb.h"
#include "administratorform.h"
#include "roomform.h"
#include "data.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QToolBar>
#include <QComboBox>

namespace paso {
namespace admin {

using namespace paso::comm;
using namespace paso::data;
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
    showWidgets(mRole);
}

void MainWindow::createMenus() {}

void MainWindow::createWidgets() {
    mWidgetChooserComboBox = new QComboBox(this);

    AbstractForm *form = new AdministratorForm(this);
    mMainWidgets.insert(SystemRole::ADMINISTRATOR, form);
    mWidgetChooserComboBox->addItem(tr("System users administration"),
                                    roleToString(SystemRole::ADMINISTRATOR));
    mWidgetChooserComboBox->addItem(tr("Room administration"),
                                    roleToString(SystemRole::ROOM_MANAGER));

    connect(mWidgetChooserComboBox, static_cast<void (QComboBox::*)(int)>(
                                        &QComboBox::currentIndexChanged),
            this, &MainWindow::onWidgetChooserCurrentIndexChanged);

    form = new RoomForm(this);
    mMainWidgets.insert(SystemRole::ROOM_MANAGER, form);
    auto toolBar = new QToolBar(tr("Main Toolbar"), this);
    toolBar->setObjectName("MainToolBar");
    mWidgetChooserSeparator = new QAction(this);
    mWidgetChooserSeparator->setSeparator(true);
    auto spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addAction(mWidgetChooserSeparator);
    toolBar->addWidget(spacer);
    toolBar->addWidget(mWidgetChooserComboBox);
    addToolBar(toolBar);
}

void MainWindow::showWidgets(SystemRole role) {
    QToolBar *toolBar = findChild<QToolBar *>("MainToolBar");
    AbstractForm *form = nullptr;
    switch (role) {
    case SystemRole::SUPER_USER:
    case SystemRole::ADMINISTRATOR:
        form = mMainWidgets[SystemRole::ADMINISTRATOR];
        break;
    case SystemRole::ROOM_MANAGER:
        form = mMainWidgets[SystemRole::ROOM_MANAGER];
        break;
    default:
        break;
    }
    if (form != nullptr) {
        setCentralWidget(nullptr);
        setCentralWidget(form);
        toolBar->insertActions(mWidgetChooserSeparator, form->toolBarActions());
    } else {
        qDebug() << "FORM IS NULL!";
    }
}

void MainWindow::onWidgetChooserCurrentIndexChanged(int index) {
    QToolBar *toolBar = findChild<QToolBar *>("MainToolBar");
    auto currentActions =
        dynamic_cast<AbstractForm *>(centralWidget())->toolBarActions();
    for (const auto &action : currentActions) {
        toolBar->removeAction(action);
    }
    showWidgets(
        stringToRole(mWidgetChooserComboBox->itemData(index).toString()));
}
}
}
