#ifndef MOCKACTIVITYWIZARDPAGE_H
#define MOCKACTIVITYWIZARDPAGE_H

#include <QWizardPage>

class MockActivityWizardPage : public QWizardPage {
    Q_OBJECT
    Q_PROPERTY(quint64 activityId READ activityId WRITE setActivityId NOTIFY
                   activityIdChanged)
    Q_PROPERTY(
        bool weekDays READ weekDays WRITE setWeekDays NOTIFY weekDaysChanged)
public:
    MockActivityWizardPage(QWidget *parent = nullptr);

    quint64 activityId() const;
    void setActivityId(const quint64 &activityId);

    bool weekDays() const;
    void setWeekDays(bool weekDays);

signals:
    void activityIdChanged();
    void weekDaysChanged();

private:
    quint64 mActivityId;
    bool mWeekDays;
};

#endif // MOCKACTIVITYWIZARDPAGE_H
