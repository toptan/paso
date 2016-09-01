#include "activitywizardnameandtypepage.h"
#include "ui_activitywizardnameandtypepage.h"

#include "data.h"

#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QDebug>
#include <QTimer>

using namespace paso::data;

namespace paso {
namespace admin {

ActivityWizardNameAndTypePage::ActivityWizardNameAndTypePage(quint64 activityId,
                                                             QWidget *parent)
    : QWizardPage(parent), ui(new Ui::ActivityWizardNameAndTypePage),
      mActivityId(activityId) {
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
    ui->weekDaysRadioButton->setChecked(true);
    ui->specificDaysRadioButton->setChecked(false);
    emit activityIdChanged();
}

quint64 ActivityWizardNameAndTypePage::activityId() const {
    return mActivityId;
}
}
}
