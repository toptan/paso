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

public:
    explicit ActivityWizardNameAndTypePage(QWidget *parent = 0);
    virtual ~ActivityWizardNameAndTypePage();

private:
    Ui::ActivityWizardNameAndTypePage *ui;
};
}
}

#endif // ACTIVITYWIZARDNAMEANDTYPEPAGE_H
