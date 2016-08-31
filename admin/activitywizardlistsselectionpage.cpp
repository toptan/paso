#include "activitywizardlistsselectionpage.h"
#include "ui_activitywizardlistsselectionpage.h"

#include "addremoveentitiesform.h"
#include "data.h"
#include "pasodb.h"

#include <QMap>
#include <QSqlError>

using namespace paso::data;
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
    auto sourceData = manager.nonActivityLists(activityId, error);
    auto destinationData = manager.activityLists(activityId, error);
    ui->entityForm->setData(tr("Non participating lists"), columns, columnNames,
                            sourceData, tr("Participating lists"), columns,
                            columnNames, destinationData);
}

bool ActivityWizardListsSelectionPage::isComplete() const {
    return !field("activityLists").toList().isEmpty();
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
