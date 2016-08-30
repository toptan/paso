#ifndef ACTIVITYWIZARDROOMSSELECTIONPAGE_H
#define ACTIVITYWIZARDROOMSSELECTIONPAGE_H

#include <QWizardPage>

namespace Ui {
class ActivityWizardRoomsSelectionPage;
}

class ActivityWizardRoomsSelectionPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit ActivityWizardRoomsSelectionPage(QWidget *parent = 0);
    ~ActivityWizardRoomsSelectionPage();

private:
    Ui::ActivityWizardRoomsSelectionPage *ui;
};

#endif // ACTIVITYWIZARDROOMSSELECTIONPAGE_H
