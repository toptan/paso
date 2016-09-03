#include "activitywizardlistsselectionpage.h"
#include "ui_activitywizardlistsselectionpage.h"

#include "addremoveentitiesform.h"
#include "data.h"
#include "entity.h"
#include "pasodb.h"

#include <QMap>
#include <QMessageBox>
#include <QSqlError>

using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::db;
using namespace paso::widget;

namespace paso {
namespace admin {

ActivityWizardListsSelectionPage::ActivityWizardListsSelectionPage(
    QWidget *parent)
    : QWizardPage(parent), ui(new Ui::ActivityWizardListsSelectionPage) {
    ui->setupUi(this);

    registerField("activityLists", this, "listIds", SIGNAL(listIdsChanged()));
    connect(ui->entityForm, &AddRemoveEntitiesForm::destinationEntitiesChanged,
            this, &ActivityWizardListsSelectionPage::setListIds);
}

ActivityWizardListsSelectionPage::~ActivityWizardListsSelectionPage() {
    delete ui;
}

void ActivityWizardListsSelectionPage::initializePage() {
    DBManager manager;
    const QStringList columns{"NAME"};
    const QMap<QString, QString> columnNames{{"NAME", tr("Name")}};
    QSqlError error;
    quint64 activityId = wizard()->field("activityId").toULongLong();
    EntityVector destinationData;
    EntityVector sourceData = manager.nonActivityLists(activityId, error);
    if (error.type() == QSqlError::NoError) {
        destinationData = manager.activityLists(activityId, error);
    }
    if (error.type() == QSqlError::NoError) {
        ui->entityForm->setData(
            tr("Non participating lists"), columns, columnNames, sourceData,
            tr("Participating lists"), columns, columnNames, destinationData);
    } else {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while loading activity lists "
                          "from the database."));
        msgBox.setDetailedText(error.text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
}

bool ActivityWizardListsSelectionPage::isComplete() const {
    return !field("activityLists").toList().isEmpty();
}

void ActivityWizardListsSelectionPage::cleanupPage() {
    // We want to preserve what user entered, so we do nothing here.
}

QVariantList ActivityWizardListsSelectionPage::listIds() const {
    return mListIds;
}

void ActivityWizardListsSelectionPage::setListIds(const QVariantList &listIds) {
    mListIds = listIds;
    emit listIdsChanged();
    emit completeChanged();
}
}
}
