#include "activitywizardrepetitivedatespage.h"
#include "ui_activitywizardrepetitivedatespage.h"

#include "itemspicker.h"

#include <QDebug>
#include <QStringList>

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
    registerField("duration*", ui->durationTimeEdit);
    connect(ui->itemsPicker, &ItemsPicker::selectedItemsChanged, [this]() {
        emit selectedDaysChanged();
        emit completeChanged();
    });
}

ActivityWizardRepetitiveDatesPage::~ActivityWizardRepetitiveDatesPage() {
    delete ui;
}

void ActivityWizardRepetitiveDatesPage::initializePage() {
    if (wizard()->field("onWeekDays").toBool()) {
        initializeWeekDaysMode();
    } else {
        initializeMonthDaysMode();
    }
    ui->startDateEdit->setDate(QDate::currentDate());
    ui->finishDateEdit->setDate(QDate::currentDate().addMonths(1));
}

bool ActivityWizardRepetitiveDatesPage::isComplete() const {
    return selectedDays().size() > 0 &&
           ui->startDateEdit->date() < ui->finishDateEdit->date() &&
           ui->durationTimeEdit->time() > QTime(0, 0, 0);
}

QList<QVariant> ActivityWizardRepetitiveDatesPage::selectedDays() const {
    QList<QVariant> retVal;
    for (auto t: ui->itemsPicker->selectedItems()) {
        retVal << t;
    }
    return retVal;
}

void ActivityWizardRepetitiveDatesPage::setSelectedDays(
    const QList<QVariant> &selectedDays) {
    QList<int> temp;
    for (const auto& t: selectedDays) {
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
