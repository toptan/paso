#ifndef ACTIVITYWIZARDROOMSSELECTIONPAGE_H
#define ACTIVITYWIZARDROOMSSELECTIONPAGE_H

#include <QVariant>
#include <QWizardPage>

namespace Ui {
class ActivityWizardRoomsSelectionPage;
}

namespace paso {
namespace admin {

///
/// \brief The ActivityWizardRoomsSelectionPage class is a page for selecting
/// activty rooms.
///
class ActivityWizardRoomsSelectionPage : public QWizardPage {
    Q_OBJECT
    /// Selected room ids.
    Q_PROPERTY(QVariantList roomIds READ roomIds WRITE setRoomIds NOTIFY
                   roomIdsChanged)
public:
    ///
    /// \brief Contructs the page.
    /// \param parent The parent widget.
    ///
    explicit ActivityWizardRoomsSelectionPage(QWidget *parent = nullptr);
    /// Destructor.
    virtual ~ActivityWizardRoomsSelectionPage();

    ///
    /// \brief Initializes the page.
    ///
    virtual void initializePage() override;
    ///
    /// \brief Returns whether page data is valid and entry can be considered as
    /// complete.
    /// \return \c true if data is valid.
    ///
    virtual bool isComplete() const override;
    ///
    /// \brief Cleans up the page.
    ///
    virtual void cleanupPage() override;

    ///
    /// \brief Returns selected room ids.
    /// \return Selected room ids.
    ///
    QVariantList roomIds() const;
    ///
    /// \brief Sets selected room ids.
    /// \param roomIds Selected room ids.
    ///
    void setRoomIds(const QVariantList &roomIds);

signals:
    ///
    /// \brief This signal is emited whenever rooms selection changes.
    ///
    void roomIdsChanged();

private:
    Ui::ActivityWizardRoomsSelectionPage *ui; //!< Generated form object.

    QVariantList mRoomIds; //!< List of selected room ids.
};
}
}
#endif // ACTIVITYWIZARDROOMSSELECTIONPAGE_H
