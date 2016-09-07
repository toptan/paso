#include "mockactivitywizardpage.h"

MockActivityWizardPage::MockActivityWizardPage(QWidget *parent)
    : mActivityId(0) {
    registerField("activityId", this, "activityId",
                  SIGNAL(activityIdChanged()));
    registerField("onWeekDays", this, "weekDays", SIGNAL(weekDaysChanged()));
    registerField("type", this, "type", SIGNAL(typeChanged()));
}

quint64 MockActivityWizardPage::activityId() const { return mActivityId; }

void MockActivityWizardPage::setActivityId(const quint64 &activityId) {
    mActivityId = activityId;
    emit activityIdChanged();
}

bool MockActivityWizardPage::weekDays() const { return mWeekDays; }

void MockActivityWizardPage::setWeekDays(bool weekDays) {
    mWeekDays = weekDays;
    emit weekDaysChanged();
}

QString MockActivityWizardPage::type() const { return mType; }

void MockActivityWizardPage::setType(const QString &type) {
    mType = type;
    emit typeChanged();
}
