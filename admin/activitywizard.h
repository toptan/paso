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

class ActivityWizard : public QWizard {
    Q_OBJECT
public:
    ///
    /// \brief The Pages enum defines all id for each wizard page.
    ///
    enum Pages {
        NameAndType,
        SingleSlot,
        MultipleSlots,
        ListSelection,
        RoomSelection
    };

    explicit ActivityWizard(QSqlRecord &record, QWidget *parent = nullptr);
    virtual ~ActivityWizard();

    virtual int nextId() const override;
    virtual void setVisible(bool visible) override;

public slots:
    virtual void accept() override;

private:
    QSqlRecord &mRecord;
    std::shared_ptr<paso::data::entity::Activity> mActivity;
};
}
}

#endif // ACTIVITYWIZARD_H
