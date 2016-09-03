#ifndef ACTIVITYWIZARDLISTSSELECTIONPAGE_H
#define ACTIVITYWIZARDLISTSSELECTIONPAGE_H

#include <QWizardPage>

namespace Ui {
class ActivityWizardListsSelectionPage;
}

namespace paso {
namespace admin {

class ActivityWizardListsSelectionPage : public QWizardPage {
    Q_OBJECT
    Q_PROPERTY(QVariantList listIds READ listIds WRITE setListIds NOTIFY
                   listIdsChanged)

public:
    explicit ActivityWizardListsSelectionPage(QWidget *parent = nullptr);
    virtual ~ActivityWizardListsSelectionPage();

    virtual void initializePage() override;
    virtual bool isComplete() const override;
    virtual void cleanupPage() override;

    QVariantList listIds() const;
    void setListIds(const QVariantList &listIds);

signals:
    void listIdsChanged();

private:
    Ui::ActivityWizardListsSelectionPage *ui;

    QVariantList mListIds;
};
}
}
#endif // ACTIVITYWIZARDLISTSSELECTIONPAGE_H
