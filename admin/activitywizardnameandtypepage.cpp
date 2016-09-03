#include "activitywizardnameandtypepage.h"
#include "ui_activitywizardnameandtypepage.h"

#include "activity.h"
#include "data.h"
#include "pasodb.h"

#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QTimer>

using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::db;

namespace paso {
namespace admin {

ActivityWizardNameAndTypePage::ActivityWizardNameAndTypePage(QWidget *parent)
    : QWizardPage(parent), ui(new Ui::ActivityWizardNameAndTypePage),
      mActivityId(0) {
    ui->setupUi(this);
    for (const auto &type : enumeratedActivityTypes.keys()) {
        ui->typeComboBox->addItem(
            QApplication::instance()->translate(
                "QObject", enumeratedActivityTypes[type].toStdString().c_str()),
            activityTypeToString(type));
    }
    connect(ui->typeComboBox, static_cast<void (QComboBox::*)(int)>(
                                  &QComboBox::currentIndexChanged),
            [=](int index) {
                ui->overlapCheckBox->setVisible(
                    ui->typeComboBox->itemData(index).toString() ==
                    "INDIVIDUAL_WORK");
            });
    connect(ui->moreThanOnceCheckBox, &QCheckBox::toggled,
            [this](bool checked) {
                ui->groupBox->setVisible(checked);
                if (checked) {
                    ui->weekDaysRadioButton->setChecked(true);
                } else {
                    ui->specificDaysRadioButton->setChecked(true);
                }
            });
    ui->overlapCheckBox->setVisible(false);
    ui->groupBox->setVisible(false);
    registerField("name*", ui->nameEdit);
    registerField("type", ui->typeComboBox, "currentData",
                  SIGNAL(currentIndexChanged(int)));
    registerField("canOverlap", ui->overlapCheckBox);
    registerField("moreThanOnce", ui->moreThanOnceCheckBox);
    registerField("activityId", this, "activityId",
                  SIGNAL(activityIdChanged()));
    registerField("onWeekDays", ui->weekDaysRadioButton);
    registerField("onMonthDays", ui->specificDaysRadioButton);
}

ActivityWizardNameAndTypePage::~ActivityWizardNameAndTypePage() { delete ui; }

void ActivityWizardNameAndTypePage::initializePage() {
    if (mActivityId == 0) {
        return;
    }
    DBManager manager;
    QSqlError error;
    auto activity = manager.getActivity(mActivityId, error);
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
    ui->nameEdit->setText(activity->name());
    ui->typeComboBox->setCurrentIndex(
        ui->typeComboBox->findData(activityTypeToString(activity->type())));
    ui->overlapCheckBox->setChecked(activity->canOverlap());
    ui->moreThanOnceCheckBox->setChecked(activity->scheduleType() !=
                                         ActivityScheduleType::ONCE);
    if (activity->scheduleType() == ActivityScheduleType::WEEK_DAYS) {
        ui->weekDaysRadioButton->setChecked(true);
        ui->specificDaysRadioButton->setChecked(false);
    } else if (activity->scheduleType() == ActivityScheduleType::MONTH_DAYS) {
        ui->weekDaysRadioButton->setChecked(false);
        ui->specificDaysRadioButton->setChecked(true);
    }
    emit activityIdChanged();
}

void ActivityWizardNameAndTypePage::cleanupPage() {
    // We want to preserve what user entered, so we do nothing here.
}

quint64 ActivityWizardNameAndTypePage::activityId() const {
    return mActivityId;
}

void ActivityWizardNameAndTypePage::setActivityId(const quint64 &activityId) {
    mActivityId = activityId;
}
}
}
