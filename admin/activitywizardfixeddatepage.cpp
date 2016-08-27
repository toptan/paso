#include "activitywizardfixeddatepage.h"
#include "ui_activitywizardfixeddatepage.h"

#include <QCalendarWidget>
#include <QDebug>

namespace paso {
namespace admin {

ActivityWizardFixedDatePage::ActivityWizardFixedDatePage(QWidget *parent)
    : QWizardPage(parent), ui(new Ui::ActivityWizardFixedDatePage) {
    ui->setupUi(this);
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime().addSecs(3600));
    ui->dateTimeEdit->setMinimumDate(QDate::currentDate());
    ui->durationEdit->setDateTime(QDateTime::currentDateTime());
    ui->durationEdit->setTime(QTime(1, 0, 0));
    registerField("selectedDateTime*", ui->dateTimeEdit);
    registerField("duration*", ui->durationEdit);
}

ActivityWizardFixedDatePage::~ActivityWizardFixedDatePage() { delete ui; }

bool ActivityWizardFixedDatePage::isComplete() const {
    return (ui->durationEdit->time() > QTime(0, 0, 0)) &&
           (ui->dateTimeEdit->dateTime() >= QDateTime::currentDateTime());
}
}
}
