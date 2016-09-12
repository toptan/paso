#ifndef ACTIVITYWIZARDREPETITIVEDATESPAGE_H
#define ACTIVITYWIZARDREPETITIVEDATESPAGE_H

#include <QList>
#include <QWizardPage>

namespace Ui {
class ActivityWizardRepetitiveDatesPage;
}

namespace paso {
namespace admin {

///
/// \brief The ActivityWizardRepetitiveDatesPage class is a page for defining
/// activity repetition.
///
class ActivityWizardRepetitiveDatesPage : public QWizardPage {
    Q_OBJECT
    /// Selected days for repetition.
    Q_PROPERTY(QList<QVariant> selectedDays READ selectedDays WRITE
                   setSelectedDays NOTIFY selectedDaysChanged)

public:
    ///
    /// \brief Contructs the page.
    /// \param parent The parent widget.
    ///
    explicit ActivityWizardRepetitiveDatesPage(QWidget *parent = nullptr);
    /// Destructor.
    virtual ~ActivityWizardRepetitiveDatesPage();

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
    /// \brief Returns list of selected days.
    /// \return Selected days.
    ///
    QList<QVariant> selectedDays() const;
    ///
    /// \brief Sets the list of selected days.
    /// \param selectedDays Selected days.
    ///
    void setSelectedDays(const QList<QVariant> &selectedDays);

signals:
    ///
    /// \brief This signal is emited whenever days selection changes.
    ///
    void selectedDaysChanged();

private:
    Ui::ActivityWizardRepetitiveDatesPage *ui; //!< Generated form object.

    ///
    /// \brief Initializes page in week days selection mode.
    ///
    void initializeWeekDaysMode();
    ///
    /// \brief Initializes page in month dates selection mode.
    ///
    void initializeMonthDaysMode();
};
}
}
#endif // ACTIVITYWIZARDREPETITIVEDATESPAGE_H
