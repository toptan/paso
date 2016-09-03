#ifndef ACTIVITYWIZARD_H
#define ACTIVITYWIZARD_H

#include <QWizard>

#include <QSqlRecord>

namespace paso {
namespace admin {

class ActivityWizard : public QWizard {
    Q_OBJECT
public:
    explicit ActivityWizard(QSqlRecord &record, QWidget *parent = nullptr);
    virtual ~ActivityWizard();

private:
    QSqlRecord &mRecord;
};
}
}

#endif // ACTIVITYWIZARD_H
