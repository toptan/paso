#ifndef ACTIVITYWIZARDFIXEDDATEPAGE_H
#define ACTIVITYWIZARDFIXEDDATEPAGE_H

#include <QWizardPage>

namespace Ui {
class ActivityWizardFixedDatePage;
}

namespace paso {
namespace admin {

///
/// \brief The ActivityWizardFixedDatePage class is a page for selecting single
/// ocurrence activity time and duration.
///
class ActivityWizardFixedDatePage : public QWizardPage {
    Q_OBJECT

public:
    ///
    /// \brief Contructs the page.
    /// \param parent The parent widget.
    ///
    explicit ActivityWizardFixedDatePage(QWidget *parent = nullptr);
    /// Destructor.
    virtual ~ActivityWizardFixedDatePage();

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

private:
    Ui::ActivityWizardFixedDatePage *ui; //!< Generated form object.
};
}
}
#endif // ACTIVITYWIZARDFIXEDDATEPAGE_H
