#include "activitywizard.h"

#include "activity.h"
#include "activityoverlapsdialog.h"
#include "activitywizardfixeddatepage.h"
#include "activitywizardlistsselectionpage.h"
#include "activitywizardnameandtypepage.h"
#include "activitywizardrepetitivedatespage.h"
#include "activitywizardroomsselectionpage.h"
#include "data.h"
#include "pasodb.h"

#include <QAbstractButton>
#include <QDebug>
#include <QLayout>
#include <QMessageBox>
#include <QSqlError>
#include <QTableView>
#include <QTimer>
#include <memory>

using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::db;
using namespace std;

namespace paso {
namespace admin {

ActivityWizard::ActivityWizard(QSqlRecord &record, QWidget *parent)
    : QWizard(parent), mRecord(record) {
    auto firstPage = new ActivityWizardNameAndTypePage(this);
    firstPage->setActivityId(record.value("ID").toULongLong());
    setPage(0, firstPage);
    setPage(1, new ActivityWizardFixedDatePage(this));
    setPage(2, new ActivityWizardRepetitiveDatesPage(this));
    setPage(3, new ActivityWizardListsSelectionPage(this));
    setPage(4, new ActivityWizardRoomsSelectionPage(this));
    setMinimumSize(640, 480);
    button(QWizard::BackButton)->setText(tr("Back"));
    button(QWizard::NextButton)->setText(tr("Next"));
    button(QWizard::CancelButton)->setText(tr("Cancel"));
    button(QWizard::FinishButton)->setText(tr("Finish"));
}

ActivityWizard::~ActivityWizard() {}

int ActivityWizard::nextId() const {
    switch (currentId()) {
    case NameAndType:
        if (field("moreThanOnce").toBool()) {
            return MultipleSlots;
        } else {
            return SingleSlot;
        }
    case SingleSlot:
    case MultipleSlots:
        if (field("type").toString() != "SPECIAL_EVENT") {
            return ListSelection;
        } else {
            return RoomSelection;
        }
    case ListSelection:
        return RoomSelection;
    case RoomSelection:
    default:
        return -1;
    }
}

void ActivityWizard::setVisible(bool visible) {
    if (visible) {
        DBManager manager;
        QSqlError error;
        if (mRecord.contains("id") && mRecord.value("id").toULongLong() != 0) {
            mActivity =
                manager.getActivity(mRecord.value("id").toULongLong(), error);
            if (error.type() != QSqlError::NoError) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Warning);
                msgBox.setText(
                    tr("There was an error while loading activity from the "
                       "database."));
                msgBox.setDetailedText(error.text());
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setDefaultButton(QMessageBox::Ok);
                msgBox.exec();
                reject();
                return;
            }
            if (!mActivity) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setText(tr("The requested activity does not exist."));
                msgBox.setInformativeText(tr("Maybe someone or system clean up "
                                             "job deleted the activity. "
                                             "Try refreshing the data."));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setDefaultButton(QMessageBox::Ok);
                msgBox.exec();
                reject();
                return;
            }
        } else {
            mActivity = make_shared<Activity>();
        }
    }
    QWizard::setVisible(visible);
}

void ActivityWizard::accept() {
    mActivity->setName(field("name").toString());
    auto activityType = stringToActivityType(field("type").toString());
    mActivity->setType(activityType);
    if (activityType == ActivityType::INDIVIDUAL_WORK) {
        mActivity->setCanOverlap(field("canOverlap").toBool());
    } else {
        mActivity->setCanOverlap(false);
    }
    if (!field("moreThanOnce").toBool()) {
        auto selectedDateTime = field("selectedDateTime").toDateTime();
        mActivity->setStartDate(selectedDateTime.date());
        mActivity->setFinishDate(selectedDateTime.date());
        mActivity->setStartTime(selectedDateTime.time());
        mActivity->setDuration(field("fixedDuration").toTime());
        mActivity->setScheduleType(ActivityScheduleType::ONCE);
    } else {
        auto scheduleType = field("onWeekDays").toBool()
                                ? ActivityScheduleType::WEEK_DAYS
                                : ActivityScheduleType::MONTH_DAYS;
        mActivity->setScheduleType(scheduleType);
        mActivity->setStartDate(field("startDate").toDate());
        mActivity->setFinishDate(field("finishDate").toDate());
        mActivity->setStartTime(field("startTime").toTime());
        mActivity->setDuration(field("repetitiveDuration").toTime());
        mActivity->setScheduledDays(field("selectedDays").toList());
    }
    mActivity->setRoomIds(field("activityRooms").toList());
    if (mActivity->type() == ActivityType::SPECIAL_EVENT) {
        mActivity->setListIds({});
    } else {
        mActivity->setListIds(field("activityLists").toList());
    }
    QSqlError error;
    DBManager manager;

    auto overlaps = manager.hasOverlaps(*mActivity, error);
    if (error.type() != QSqlError::NoError) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while checking whether activity "
                          "overlaps with some other one."));
        msgBox.setDetailedText(error.text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        reject();
        return;
    }

    if (overlaps) {
        QMessageBox msgBox(QMessageBox::NoIcon, tr("Activity has overlaps"),
                           tr("The activity is overlaping with other "
                              "activities. Do you want to see them?"),
                           QMessageBox::Yes | QMessageBox::No, this);
        msgBox.setWindowModality(Qt::ApplicationModal);
        msgBox.setDefaultButton(QMessageBox::Yes);
        msgBox.setButtonText(QMessageBox::Yes, tr("Yes"));
        msgBox.setButtonText(QMessageBox::No, tr("No"));
        if (msgBox.exec() == QMessageBox::Yes) {
            if (showOverlapsDialog() != QDialog::Accepted) {
                return;
            }
        }
    }

    if (!manager.saveActivity(*mActivity, error)) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while saving activity to the "
                          "database."));
        msgBox.setDetailedText(error.text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        reject();
        return;
    }
    QWizard::accept();
}

int ActivityWizard::showOverlapsDialog() {
    ActivityOverlapsDialog dlg(*mActivity, this);
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.setWindowTitle(tr("Activity %1 overlaps").arg(mActivity->name()));
    return dlg.exec();
}
}
}
