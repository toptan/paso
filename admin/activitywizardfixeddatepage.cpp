#include "activitywizardfixeddatepage.h"
#include "ui_activitywizardfixeddatepage.h"

#include "activity.h"
#include "data.h"
#include "pasodb.h"

#include <QCalendarWidget>
#include <QDebug>
#include <QMessageBox>
#include <QSqlError>

using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::db;

namespace paso {
namespace admin {

ActivityWizardFixedDatePage::ActivityWizardFixedDatePage(QWidget *parent)
    : QWizardPage(parent), ui(new Ui::ActivityWizardFixedDatePage) {
    ui->setupUi(this);
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime().addSecs(3600));
    ui->durationEdit->setDateTime(QDateTime::currentDateTime());
    ui->durationEdit->setTime(QTime(1, 0, 0));
    registerField("selectedDateTime*", ui->dateTimeEdit);
    registerField("duration*", ui->durationEdit);
}

ActivityWizardFixedDatePage::~ActivityWizardFixedDatePage() { delete ui; }

void ActivityWizardFixedDatePage::initializePage() {
    quint64 activityId = wizard()->field("activityId").toULongLong();
    if (activityId == 0) {
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
    ui->durationEdit->setTime(activity->duration());
    QDateTime temp(activity->startDate(), activity->startTime());
    ui->dateTimeEdit->setDateTime(temp);
}

bool ActivityWizardFixedDatePage::isComplete() const {
    return ui->durationEdit->time() > QTime(0, 0, 0);
}
}
}
