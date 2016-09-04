#include "activitywizardrepetitivedatespage.h"
#include "ui_activitywizardrepetitivedatespage.h"

#include "itemspicker.h"

#include "activity.h"
#include "data.h"
#include "pasodb.h"

#include <QMessageBox>
#include <QSqlError>
#include <QStringList>

using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::db;
using namespace paso::widget;

namespace paso {
namespace admin {

ActivityWizardRepetitiveDatesPage::ActivityWizardRepetitiveDatesPage(
    QWidget *parent)
    : QWizardPage(parent), ui(new Ui::ActivityWizardRepetitiveDatesPage) {
    ui->setupUi(this);
    ui->itemsPicker->setColumnCount(7);
    registerField("selectedDays*", this, "selectedDays",
                  SIGNAL(selectedDaysChanged()));
    registerField("startDate*", ui->startDateEdit);
    registerField("finishDate*", ui->finishDateEdit);
    registerField("repetitiveDuration*", ui->durationTimeEdit);
    registerField("startTime", ui->startTimeEdit);
    connect(ui->itemsPicker, &ItemsPicker::selectedItemsChanged, [this]() {
        emit selectedDaysChanged();
        emit completeChanged();
    });
}

ActivityWizardRepetitiveDatesPage::~ActivityWizardRepetitiveDatesPage() {
    delete ui;
}

void ActivityWizardRepetitiveDatesPage::initializePage() {
    auto activityId = wizard()->field("activityId").toULongLong();
    if (activityId == 0) {
        if (wizard()->field("onWeekDays").toBool()) {
            initializeWeekDaysMode();
        } else {
            initializeMonthDaysMode();
        }
        ui->startDateEdit->setDate(QDate::currentDate());
        ui->finishDateEdit->setDate(QDate::currentDate().addMonths(1));
        return;
    }
    DBManager manager;
    QSqlError error;
    auto activity = manager.getActivity(activityId, error);
    if (error.type() != QSqlError::NoError) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while loading activity from the "
                          "database."));
        msgBox.setDetailedText(error.text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    if (!activity) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText(tr("The requested activity does not exist."));
        msgBox.setInformativeText(
            tr("Maybe someone or system clean up job deleted the activity. "
               "Try refreshing the data."));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    QList<int> temp;
    for (const auto &x : activity->scheduledDays()) {
        temp << x.toInt();
    }

    if (wizard()->field("onWeekDays").toBool()) {
        initializeWeekDaysMode();
        if (activity->scheduleType() == ActivityScheduleType::WEEK_DAYS) {
            setSelectedDays(activity->scheduledDays());
        }
    } else {
        initializeMonthDaysMode();
        if (activity->scheduleType() == ActivityScheduleType::MONTH_DAYS) {
            setSelectedDays(activity->scheduledDays());
        }
    }

    ui->startDateEdit->setDate(activity->startDate());
    ui->startTimeEdit->setTime(activity->startTime());
    ui->finishDateEdit->setDate(activity->finishDate());
    ui->durationTimeEdit->setTime(activity->duration());
}

bool ActivityWizardRepetitiveDatesPage::isComplete() const {
    return selectedDays().size() > 0 &&
           ui->startDateEdit->date() < ui->finishDateEdit->date() &&
           ui->durationTimeEdit->time() > QTime(0, 0, 0);
}

void ActivityWizardRepetitiveDatesPage::cleanupPage() {
    // We want to preserve what user entered, so we do nothing here.
}

QList<QVariant> ActivityWizardRepetitiveDatesPage::selectedDays() const {
    QList<QVariant> retVal;
    for (auto t : ui->itemsPicker->selectedItems()) {
        retVal << t;
    }
    return retVal;
}

void ActivityWizardRepetitiveDatesPage::setSelectedDays(
    const QList<QVariant> &selectedDays) {
    QList<int> temp;
    for (const auto &t : selectedDays) {
        temp << t.toInt();
    }
    ui->itemsPicker->setSelectedItems(temp);
    emit completeChanged();
}

void ActivityWizardRepetitiveDatesPage::initializeWeekDaysMode() {
    const QStringList itemLabels{tr("Mo"), tr("Tu"), tr("We"), tr("Th"),
                                 tr("Fr"), tr("Sa"), tr("Su")};
    ui->pickerLabel->setText(tr("Week days"));
    ui->itemsPicker->setItemLabels(itemLabels);
}

void ActivityWizardRepetitiveDatesPage::initializeMonthDaysMode() {
    QStringList itemLabels;
    for (auto i = 1; i <= 31; i++) {
        itemLabels << QString("%1").arg(i);
    }
    ui->pickerLabel->setText(tr("Month days"));
    ui->itemsPicker->setItemLabels(itemLabels);
}
}
}
