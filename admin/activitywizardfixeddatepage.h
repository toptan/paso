#ifndef ACTIVITYWIZARDFIXEDDATEPAGE_H
#define ACTIVITYWIZARDFIXEDDATEPAGE_H

#include <QWizardPage>

namespace Ui {
class ActivityWizardFixedDatePage;
}

namespace paso {
namespace admin {

class ActivityWizardFixedDatePage : public QWizardPage {
    Q_OBJECT

public:
    explicit ActivityWizardFixedDatePage(QWidget *parent = 0);
    virtual ~ActivityWizardFixedDatePage();

    virtual bool isComplete() const override;

private:
    Ui::ActivityWizardFixedDatePage *ui;
};
}
}
#endif // ACTIVITYWIZARDFIXEDDATEPAGE_H