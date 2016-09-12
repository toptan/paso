#ifndef ACTIVITYWIZARDLISTSSELECTIONPAGE_H
#define ACTIVITYWIZARDLISTSSELECTIONPAGE_H

#include <QWizardPage>

namespace Ui {
class ActivityWizardListsSelectionPage;
}

namespace paso {
namespace admin {

///
/// \brief The ActivityWizardListsSelectionPage class is a page for selecting
/// activity lists.
///
class ActivityWizardListsSelectionPage : public QWizardPage {
    Q_OBJECT
    /// Selected list ids.
    Q_PROPERTY(QVariantList listIds READ listIds WRITE setListIds NOTIFY
                   listIdsChanged)

public:
    ///
    /// \brief Contructs the page.
    /// \param parent The parent widget.
    ///
    explicit ActivityWizardListsSelectionPage(QWidget *parent = nullptr);
    /// Destructor.
    virtual ~ActivityWizardListsSelectionPage();

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
    /// \brief Returns selected list ids.
    /// \return Selected list ids.
    ///
    QVariantList listIds() const;
    ///
    /// \brief Sets selected list ids.
    /// \param listIds Selected list ids.
    ///
    void setListIds(const QVariantList &listIds);

signals:
    ///
    /// \brief The signal is emited whenever lists selection changes.
    ///
    void listIdsChanged();

private:
    Ui::ActivityWizardListsSelectionPage *ui; //!< Generated form object.

    QVariantList mListIds; //!< List of selected list ids.
};
}
}
#endif // ACTIVITYWIZARDLISTSSELECTIONPAGE_H
