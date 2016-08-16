#include "listdetailsdialog.h"
#include "ui_listdetailsdialog.h"

#include "pasodb.h"

#include <QPushButton>

using namespace std;
using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::db;
using namespace paso::model;

namespace paso {
namespace admin {

struct ListDetailsDialog::ListDetailsDialogPrivate {
    explicit ListDetailsDialogPrivate(const List &list)
        : list(list), manager(DEFAULT_DB_NAME) {}

    const List list;

    DBManager manager;
};

ListDetailsDialog::ListDetailsDialog(const List &list, QWidget *parent)
    : QDialog(parent), ui(new Ui::ListDetailsDialog),
      mPrivate(new ListDetailsDialogPrivate(list)) {
    ui->setupUi(this);
    if (list.system()) {
        ui->typeLabel->setText(tr("System list:"));
        ui->addRemoveEntitiesForm->setReadOnly();
    } else if (list.permanent()) {
        ui->typeLabel->setText(tr("Permanent list:"));
    } else {
        ui->typeLabel->setText(tr("List:"));
    }
    ui->nameLabel->setText(list.name());
    ui->buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));
    auto refreshButton = new QPushButton(tr("Refresh"), ui->buttonBox);
    refreshButton->setObjectName("REFRESH_BUTTON");
    ui->buttonBox->addButton(refreshButton, QDialogButtonBox::ResetRole);
    if (!list.system()) {
        auto importButton =
            new QPushButton(tr("Import Students"), ui->buttonBox);
        importButton->setObjectName("IMPORT_BUTTON");
        ui->buttonBox->addButton(importButton,
                                 QDialogButtonBox::DestructiveRole);
        ui->buttonBox->button(QDialogButtonBox::Save)->setText(tr("Save"));
    } else {
        ui->buttonBox->removeButton(
            ui->buttonBox->button(QDialogButtonBox::Save));
    }
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this,
            &ListDetailsDialog::onButtonBoxButtonClicked);
    loadData();
}

ListDetailsDialog::~ListDetailsDialog() { delete ui; }

void ListDetailsDialog::reject() { QDialog::reject(); }

void ListDetailsDialog::onButtonBoxButtonClicked(QAbstractButton *button) {}

void ListDetailsDialog::refresh() {}

void ListDetailsDialog::loadData() {}

void ListDetailsDialog::saveData() {}

void ListDetailsDialog::importListStudents() {}

void ListDetailsDialog::onImportFileSelected(const QString &fileName) {}
}
}
