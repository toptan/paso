#ifndef ACTIVITYWIZARDREPETITIVEDATESPAGE_H
#define ACTIVITYWIZARDREPETITIVEDATESPAGE_H

#include <QList>
#include <QWizardPage>

namespace Ui {
class ActivityWizardRepetitiveDatesPage;
}

namespace paso {
namespace admin {

class ActivityWizardRepetitiveDatesPage : public QWizardPage {
    Q_OBJECT
    Q_PROPERTY(QList<QVariant> selectedDays READ selectedDays WRITE
                   setSelectedDays NOTIFY selectedDaysChanged)

public:
    explicit ActivityWizardRepetitiveDatesPage(QWidget *parent = 0);
    virtual ~ActivityWizardRepetitiveDatesPage();

    virtual void initializePage() override;
    virtual bool isComplete() const override;

    QList<QVariant> selectedDays() const;
    void setSelectedDays(const QList<QVariant> &selectedDays);

signals:
    void selectedDaysChanged();

private:
    Ui::ActivityWizardRepetitiveDatesPage *ui;

    void initializeWeekDaysMode();
    void initializeMonthDaysMode();
};
}
}
#endif // ACTIVITYWIZARDREPETITIVEDATESPAGE_H
