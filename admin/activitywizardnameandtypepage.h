#ifndef ACTIVITYWIZARDNAMEANDTYPEPAGE_H
#define ACTIVITYWIZARDNAMEANDTYPEPAGE_H

#include <QWizardPage>

namespace Ui {
class ActivityWizardNameAndTypePage;
}

namespace paso {
namespace admin {

class ActivityWizardNameAndTypePage : public QWizardPage {
    Q_OBJECT
    Q_PROPERTY(quint64 activityId READ activityId WRITE setActivityId NOTIFY
                   activityIdChanged)
public:
    explicit ActivityWizardNameAndTypePage(QWidget *parent = 0);
    virtual ~ActivityWizardNameAndTypePage();

    virtual void initializePage() override;
    virtual int nextId() const override;

    quint64 activityId() const;
    void setActivityId(const quint64 &activityId);

signals:
    void activityIdChanged();

private:
    Ui::ActivityWizardNameAndTypePage *ui;

    quint64 mActivityId;
};
}
}

#endif // ACTIVITYWIZARDNAMEANDTYPEPAGE_H
