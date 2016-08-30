#include "activitywizardroomsselectionpage.h"
#include "ui_activitywizardroomsselectionpage.h"

#include "addremoveentitiesform.h"
#include "data.h"
#include "pasodb.h"

#include <QDebug>
#include <QMap>
#include <QSqlError>

using namespace paso::data;
using namespace paso::db;
using namespace paso::widget;

namespace paso {
namespace admin {

ActivityWizardRoomsSelectionPage::ActivityWizardRoomsSelectionPage(
    QWidget *parent)
    : QWizardPage(parent), ui(new Ui::ActivityWizardRoomsSelectionPage) {
    ui->setupUi(this);

    registerField("activityRooms", this, "roomIds", SIGNAL(roomIdsChanged()));
    connect(ui->entityForm, &AddRemoveEntitiesForm::destinationEntitiesChanged,
            this,
            &ActivityWizardRoomsSelectionPage::setRoomIds);
}

ActivityWizardRoomsSelectionPage::~ActivityWizardRoomsSelectionPage() {
    delete ui;
}

void ActivityWizardRoomsSelectionPage::initializePage() {
    DBManager manager;
    const QStringList columns{"name", "room_number"};
    const QMap<QString, QString> columnNames{
        {"name", tr("Name")}, {"room_number", tr("Room number")}};
    QSqlError error;
    quint64 activityId = wizard()->field("activityId").toULongLong();
    auto sourceData = manager.nonActivityRooms(activityId, error);
    auto destinationData = manager.activityRooms(activityId, error);
    ui->entityForm->setData(tr("Non assigned rooms"), columns, columnNames,
                            sourceData, tr("Assigned rooms"), columns,
                            columnNames, destinationData);
}

bool ActivityWizardRoomsSelectionPage::isComplete() const {
    return !field("activityRooms").toList().isEmpty();
}

QVariantList ActivityWizardRoomsSelectionPage::roomIds() const {
    return mRoomIds;
}

void ActivityWizardRoomsSelectionPage::setRoomIds(const QVariantList &roomIds) {
    mRoomIds = roomIds;
    emit roomIdsChanged();
}
}
}
