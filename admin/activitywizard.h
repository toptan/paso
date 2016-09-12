#ifndef ACTIVITYWIZARD_H
#define ACTIVITYWIZARD_H

#include <QSqlRecord>
#include <QWizard>
#include <memory>

namespace paso {
namespace data {
namespace entity {
class Activity;
}
}
}

namespace paso {
namespace admin {

///
/// \brief The ActivityWizard class is a wizard for editing activities.
///
class ActivityWizard : public QWizard {
    Q_OBJECT
public:
    ///
    /// \brief The Pages enum defines all id for each wizard page.
    ///
    enum Pages {
        NameAndType,   //!< Name and type page.
        SingleSlot,    //!< Single ocurrence time selection page.
        MultipleSlots, //!< Repetitive activity time slots selection page.
        ListSelection, //!< Lists selection page.
        RoomSelection  //!< Rooms selection page.
    };

    ///
    /// \brief Constructs activity wizard for given activity SQL record.
    /// \param record The activity SQL record.
    /// \param parent The parent widget.
    ///
    explicit ActivityWizard(QSqlRecord &record, QWidget *parent = nullptr);
    /// Destructor.
    virtual ~ActivityWizard();

    ///
    /// \brief Returns id of the next page to be displayed.
    /// \return Id of the next page.
    ///
    virtual int nextId() const override;
    ///
    /// \brief Overriden method to load activity data just before showing.
    /// \param visible Whether to set wizard visible or not.
    ///
    virtual void setVisible(bool visible) override;

public slots:
    ///
    /// \brief Overriden slot to save activity.
    ///
    virtual void accept() override;

private:
    QSqlRecord &mRecord; //!< The reference to the activity SQL record.

    /// The activity being edited.
    std::shared_ptr<paso::data::entity::Activity> mActivity;
};
}
}

#endif // ACTIVITYWIZARD_H
