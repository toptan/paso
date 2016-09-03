#ifndef ACTIVITYWIZARDROOMSSELECTIONPAGE_H
#define ACTIVITYWIZARDROOMSSELECTIONPAGE_H

#include <QVariant>
#include <QWizardPage>

namespace Ui {
class ActivityWizardRoomsSelectionPage;
}

namespace paso {
namespace admin {

class ActivityWizardRoomsSelectionPage : public QWizardPage {
    Q_OBJECT
    Q_PROPERTY(QVariantList roomIds READ roomIds WRITE setRoomIds NOTIFY
                   roomIdsChanged)
public:
    explicit ActivityWizardRoomsSelectionPage(QWidget *parent = nullptr);
    virtual ~ActivityWizardRoomsSelectionPage();

    virtual void initializePage() override;
    virtual bool isComplete() const override;
    virtual void cleanupPage() override;

    QVariantList roomIds() const;
    void setRoomIds(const QVariantList &roomIds);

signals:
    void roomIdsChanged();

private:
    Ui::ActivityWizardRoomsSelectionPage *ui;

    QVariantList mRoomIds;
};
}
}
#endif // ACTIVITYWIZARDROOMSSELECTIONPAGE_H
