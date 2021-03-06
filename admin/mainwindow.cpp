#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "activityform.h"
#include "courseform.h"
#include "data.h"
#include "listform.h"
#include "logindialog.h"
#include "pasodb.h"
#include "roomform.h"
#include "studentform.h"
#include "systemusersform.h"
#include "teacherform.h"

#include <QComboBox>
#include <QDebug>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStackedWidget>
#include <QToolBar>

namespace paso {
namespace admin {

using namespace paso::comm;
using namespace paso::data;
using namespace paso::db;
using namespace paso::widget;

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

void MainWindow::createDbConnection(const QString &connectionName,
                                    const LoginResponse &response) {
    if (!QSqlDatabase::contains(connectionName)) {
        auto db = QSqlDatabase::addDatabase(response.dbType(), connectionName);
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
    }
}

void MainWindow::loginFinished(const LoginResponse &response) {
    mRole = response.systemUser().role();
    createDbConnection(DEFAULT_DB_NAME, response);
    createWidgets();
    if (mMainWidget->count() > 0) {
        onWidgetChooserCurrentIndexChanged(0);
    }
}

void MainWindow::createWidgets() {
    auto toolBar = new QToolBar(tr("Main Toolbar"), this);
    toolBar->setObjectName("MainToolBar");

    mWidgetChooserSeparator = new QAction(this);
    mWidgetChooserSeparator->setSeparator(true);
    toolBar->addAction(mWidgetChooserSeparator);

    auto spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolBar->addWidget(spacer);

    mWidgetChooserComboBox = new QComboBox(this);
    toolBar->addWidget(mWidgetChooserComboBox);
    addToolBar(toolBar);

    switch (mRole) {
    case SystemRole::SUPER_USER:
        setupAdministratorUI();
        setupManagerUI();
        break;
    case SystemRole::ADMINISTRATOR:
        setupAdministratorUI();
        break;
    case SystemRole::MANAGER:
        setupManagerUI();
        break;
    default:
        break;
    }
    if (mMainWidget->count() > 0) {
        mWidgetChooserComboBox->setCurrentIndex(0);
        connect(mWidgetChooserComboBox, static_cast<void (QComboBox::*)(int)>(
                                            &QComboBox::currentIndexChanged),
                this, &MainWindow::onWidgetChooserCurrentIndexChanged);
    }
    for (auto i = 0; i < mWidgetChooserComboBox->count(); i++) {
        auto action = new QAction(this);
        action->setShortcut(Qt::CTRL | (Qt::Key_1 + i));
        connect(action, &QAction::triggered,
                [this, i]() { mWidgetChooserComboBox->setCurrentIndex(i); });
        addAction(action);
    }
}

void MainWindow::setupAdministratorUI() {
    auto systemUsersForm = new SystemUsersForm(this);
    mMainWidget->addWidget(systemUsersForm);
    mWidgetChooserComboBox->addItem(tr("System users administration"));
    auto roomForm = new RoomForm(this);
    mMainWidget->addWidget(roomForm);
    mWidgetChooserComboBox->addItem(tr("Room administration"));
}

void MainWindow::setupManagerUI() {
    auto courseForm = new CourseForm(this);
    mMainWidget->addWidget(courseForm);
    mWidgetChooserComboBox->addItem(tr("Course administration"));
    auto studentForm = new StudentForm(this);
    mMainWidget->addWidget(studentForm);
    mWidgetChooserComboBox->addItem(tr("Student administration"));
    auto teacherForm = new TeacherForm(this);
    mMainWidget->addWidget(teacherForm);
    mWidgetChooserComboBox->addItem(tr("Teacher administration"));
    auto listForm = new ListForm(this);
    mMainWidget->addWidget(listForm);
    mWidgetChooserComboBox->addItem(tr("List administration"));
    auto activityForm = new ActivityForm(this);
    mMainWidget->addWidget(activityForm);
    mWidgetChooserComboBox->addItem(tr("Activity administration"));
}

void MainWindow::onWidgetChooserCurrentIndexChanged(int index) {
    QToolBar *toolBar = findChild<QToolBar *>("MainToolBar");
    auto currentActions = dynamic_cast<AbstractForm *>(
                              mMainWidget->widget(mMainWidget->currentIndex()))
                              ->toolBarActions();
    for (const auto &action : currentActions) {
        toolBar->removeAction(action);
    }
    toolBar->insertActions(
        mWidgetChooserSeparator,
        dynamic_cast<AbstractForm *>(mMainWidget->widget(index))
            ->toolBarActions());
    mMainWidget->setCurrentIndex(index);
}
}
}
