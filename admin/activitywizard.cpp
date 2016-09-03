#include "activitywizard.h"

#include "activitywizardfixeddatepage.h"
#include "activitywizardlistsselectionpage.h"
#include "activitywizardnameandtypepage.h"
#include "activitywizardrepetitivedatespage.h"
#include "activitywizardroomsselectionpage.h"

namespace paso {
namespace admin {

ActivityWizard::ActivityWizard(QSqlRecord &record, QWidget *parent)
    : QWizard(parent), mRecord(record) {
    auto firstPage = new ActivityWizardNameAndTypePage(this);
    firstPage->setActivityId(record.value("ID").toULongLong());
    setPage(0, firstPage);
    setPage(1, new ActivityWizardFixedDatePage(this));
    setPage(2, new ActivityWizardRepetitiveDatesPage(this));
    setPage(3, new ActivityWizardListsSelectionPage(this));
    setPage(4, new ActivityWizardRoomsSelectionPage(this));
    setMinimumSize(640, 480);
}

ActivityWizard::~ActivityWizard() {}

int ActivityWizard::nextId() const {
    switch (currentId()) {
    case NameAndType:
        if (field("moreThanOnce").toBool()) {
            return MultipleSlots;
        } else {
            return SingleSlot;
        }
    case SingleSlot:
    case MultipleSlots:
        return ListSelection;
    case ListSelection:
        return RoomSelection;
    case RoomSelection:
    default:
        return -1;
    }
}
}
}
