#ifndef ACTIVITYWIZARD_H
#define ACTIVITYWIZARD_H

#include <QWizard>

#include <QSqlRecord>

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

private:
    QSqlRecord &mRecord;
};
}
}

#endif // ACTIVITYWIZARD_H
