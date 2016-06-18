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
#include <QStackedWidget>

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
    mMainWidget = new QStackedWidget(this);
    setCentralWidget(mMainWidget);
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
    mMainWidget->addWidget(form);
    mWidgetChooserComboBox->addItem(tr("System users administration"),
                                    roleToString(SystemRole::ADMINISTRATOR));
    mWidgetChooserComboBox->addItem(tr("Room administration"),
                                    roleToString(SystemRole::ROOM_MANAGER));

    connect(mWidgetChooserComboBox, static_cast<void (QComboBox::*)(int)>(
                                        &QComboBox::currentIndexChanged),
            this, &MainWindow::onWidgetChooserCurrentIndexChanged);

    form = new RoomForm(this);
    mMainWidgets.insert(SystemRole::ROOM_MANAGER, form);
    mMainWidget->addWidget(form);
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
    int index = 0;
    switch (role) {
    case SystemRole::SUPER_USER:
    case SystemRole::ADMINISTRATOR:
        form = mMainWidgets[SystemRole::ADMINISTRATOR];
        index = 0;
        break;
    case SystemRole::ROOM_MANAGER:
        form = mMainWidgets[SystemRole::ROOM_MANAGER];
        index = 1;
        break;
    default:
        break;
    }
    if (form != nullptr) {
        mMainWidget->setCurrentIndex(index);
        toolBar->insertActions(mWidgetChooserSeparator, form->toolBarActions());
    }
}

void MainWindow::onWidgetChooserCurrentIndexChanged(int index) {
    QToolBar *toolBar = findChild<QToolBar *>("MainToolBar");
    auto currentActions = dynamic_cast<AbstractForm *>(
                              mMainWidget->widget(mMainWidget->currentIndex()))
                              ->toolBarActions();
    for (const auto &action : currentActions) {
        toolBar->removeAction(action);
    }
    showWidgets(
        stringToRole(mWidgetChooserComboBox->itemData(index).toString()));
}
}
}
