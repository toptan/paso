#include "activityoverlapsdialog.h"
#include "ui_activityoverlapsdialog.h"

#include "pasodb.h"
#include "stablerownumbersortfilterproxymodel.h"

#include <QPushButton>
#include <QSqlQueryModel>

using namespace paso::data::entity;
using namespace paso::db;
using namespace paso::model;

namespace paso {
namespace admin {

ActivityOverlapsDialog::ActivityOverlapsDialog(const Activity &activity,
                                               QWidget *parent)
    : QDialog(parent), ui(new Ui::ActivityOverlapsDialog) {
    ui->setupUi(this);
    auto model = new QSqlQueryModel(this);
    auto query = Activity::hasOverlapsQuery(
        QSqlDatabase::database(DEFAULT_DB_NAME), activity);
    query.exec();
    model->setQuery(query);
    auto proxyModel = new StableRowNumberSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    ui->tableView->setModel(proxyModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    ui->tableView->hideColumn(1);
    ui->tableView->horizontalHeader()->model()->setHeaderData(
        0, Qt::Horizontal, tr("Room"), Qt::DisplayRole);
    ui->tableView->horizontalHeader()->model()->setHeaderData(
        2, Qt::Horizontal, tr("Activity"), Qt::DisplayRole);
    ui->tableView->horizontalHeader()->model()->setHeaderData(
        3, Qt::Horizontal, tr("Start time"), Qt::DisplayRole);
    ui->tableView->horizontalHeader()->model()->setHeaderData(
        4, Qt::Horizontal, tr("Finish time"), Qt::DisplayRole);
    ui->tableView->sortByColumn(0, Qt::SortOrder::AscendingOrder);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Save activity"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
}

ActivityOverlapsDialog::~ActivityOverlapsDialog() { delete ui; }
}
}
