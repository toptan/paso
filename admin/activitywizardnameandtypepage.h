#ifndef ACTIVITYWIZARDNAMEANDTYPEPAGE_H
#define ACTIVITYWIZARDNAMEANDTYPEPAGE_H

#include <QWizardPage>

namespace Ui {
class ActivityWizardNameAndTypePage;
}

namespace paso {
namespace admin {

///
/// \brief The ActivityWizardNameAndTypePage class is a page for entering
/// activity name and type.
///
class ActivityWizardNameAndTypePage : public QWizardPage {
    Q_OBJECT
    /// The id of activity being edited.
    Q_PROPERTY(quint64 activityId READ activityId WRITE setActivityId NOTIFY
                   activityIdChanged)
public:
    ///
    /// \brief Contructs the page.
    /// \param parent The parent widget.
    ///
    explicit ActivityWizardNameAndTypePage(QWidget *parent = nullptr);
    /// Destructor.
    virtual ~ActivityWizardNameAndTypePage();

    ///
    /// \brief Initializes the page.
    ///
    virtual void initializePage() override;
    ///
    /// \brief Cleans up the page.
    ///
    virtual void cleanupPage() override;

    ///
    /// \brief Returns id of the activity being edited.
    /// \return Activity id.
    ///
    quint64 activityId() const;
    ///
    /// \brief Sets id of the activity to be edited.
    /// \param activityId Activity id.
    ///
    void setActivityId(const quint64 &activityId);

signals:
    ///
    /// \brief This signal is emited whenever activity id changes.
    ///
    void activityIdChanged();

private:
    Ui::ActivityWizardNameAndTypePage *ui; //!< Generated form object.

    quint64 mActivityId; //!< The id of activity being edited.
};
}
}

#endif // ACTIVITYWIZARDNAMEANDTYPEPAGE_H
