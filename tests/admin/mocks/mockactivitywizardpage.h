#ifndef MOCKACTIVITYWIZARDPAGE_H
#define MOCKACTIVITYWIZARDPAGE_H

#include <QWizardPage>

class MockActivityWizardPage : public QWizardPage {
    Q_OBJECT
    Q_PROPERTY(quint64 activityId READ activityId WRITE setActivityId NOTIFY
                   activityIdChanged)
    Q_PROPERTY(
        bool weekDays READ weekDays WRITE setWeekDays NOTIFY weekDaysChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
public:
    MockActivityWizardPage(QWidget *parent = nullptr);

    quint64 activityId() const;
    void setActivityId(const quint64 &activityId);

    bool weekDays() const;
    void setWeekDays(bool weekDays);

    QString type() const;
    void setType(const QString &type);

signals:
    void activityIdChanged();
    void weekDaysChanged();
    void typeChanged();

private:
    quint64 mActivityId;
    bool mWeekDays;
    QString mType;
};

#endif // MOCKACTIVITYWIZARDPAGE_H
