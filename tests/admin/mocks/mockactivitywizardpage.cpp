#include "mockactivitywizardpage.h"

MockActivityWizardPage::MockActivityWizardPage(QWidget *parent)
    : mActivityId(0) {
    registerField("activityId", this, "activityId",
                  SIGNAL(activityIdChanged()));
    registerField("onWeekDays", this, "weekDays", SIGNAL(weekDaysChanged()));
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
