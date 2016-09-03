#include "activitywizardroomsselectionpage.h"
#include "ui_activitywizardroomsselectionpage.h"

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

ActivityWizardRoomsSelectionPage::ActivityWizardRoomsSelectionPage(
    QWidget *parent)
    : QWizardPage(parent), ui(new Ui::ActivityWizardRoomsSelectionPage) {
    ui->setupUi(this);

    registerField("activityRooms", this, "roomIds", SIGNAL(roomIdsChanged()));
    connect(ui->entityForm, &AddRemoveEntitiesForm::destinationEntitiesChanged,
            this, &ActivityWizardRoomsSelectionPage::setRoomIds);
}

ActivityWizardRoomsSelectionPage::~ActivityWizardRoomsSelectionPage() {
    delete ui;
}

void ActivityWizardRoomsSelectionPage::initializePage() {
    DBManager manager;
    const QStringList columns{"NAME", "ROOM_NUMBER"};
    const QMap<QString, QString> columnNames{
        {"NAME", tr("Name")}, {"ROOM_NUMBER", tr("Room number")}};
    QSqlError error;
    quint64 activityId = wizard()->field("activityId").toULongLong();
    EntityVector destinationData;
    EntityVector sourceData = manager.nonActivityRooms(activityId, error);
    if (error.type() == QSqlError::NoError) {
        destinationData = manager.activityRooms(activityId, error);
    }
    if (error.type() == QSqlError::NoError) {
        ui->entityForm->setData(tr("Non assigned rooms"), columns, columnNames,
                                sourceData, tr("Assigned rooms"), columns,
                                columnNames, destinationData);
    } else {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while loading activity rooms "
                          "from the database."));
        msgBox.setDetailedText(error.text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
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
    emit completeChanged();
}
}
}
