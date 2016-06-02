#include "administratorform.h"
#include "ui_administratorform.h"

#include "pasodb.h"

#include <QSqlTableModel>

#include <QDebug>

using namespace paso::db;

namespace paso {
namespace admin {

AdministratorForm::AdministratorForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::AdministratorForm) {
    ui->setupUi(this);
    mModel =
        new SystemUserTableModel(this, QSqlDatabase::database(DEFAULT_DB_NAME));
    ui->tableView->setModel(mModel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    ui->recordDetails->setupForRecord(mModel->record());

    connect(ui->tableView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            [this](const QModelIndex &selected, const QModelIndex &) {
                ui->recordDetails->onDisplayRecord(
                    mModel->record(selected.row()));
            });

    connect(ui->recordDetails, &RecordDisplayWidget::editCanceled, this,
            &AdministratorForm::editCanceled);

    QAction *action = new QAction(tr("New system user"), this);
    mActions.push_back(action);
    action = new QAction(tr("Edit system user"), this);
    connect(action, &QAction::triggered, this,
            &AdministratorForm::editSelected);
    mActions.push_back(action);
    action = new QAction(tr("Delete system user"), this);
    mActions.push_back(action);
    action = new QAction(this);
    action->setSeparator(true);
    mActions.push_back(action);
    action = new QAction(tr("Refresh data"), this);
    connect(action, &QAction::triggered, mModel, &SystemUserTableModel::select);
    connect(action, &QAction::triggered, ui->recordDetails,
            &RecordDisplayWidget::clearData);
    mActions.push_back(action);
}

AdministratorForm::~AdministratorForm() { delete ui; }

const QList<QAction *> &AdministratorForm::toolBarActions() const {
    return mActions;
}

void AdministratorForm::editSelected() {
    if (!ui->tableView->selectionModel()->currentIndex().isValid()) {
        return;
    }
    ui->tableView->setDisabled(true);
    ui->recordDetails->onEditRecord(
        mModel->record(ui->tableView->selectionModel()->currentIndex().row()));
}

void AdministratorForm::editCanceled() {
    ui->tableView->setDisabled(false);
    ui->tableView->setFocus();
}
}
}
