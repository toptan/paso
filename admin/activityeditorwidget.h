#ifndef ACTIVITYEDITORWIDGET_H
#define ACTIVITYEDITORWIDGET_H

#include "recordeditorwidget.h"

class QListView;
class QTableView;
class QSqlQueryModel;

namespace paso {
namespace model {
class EntityTableModel;
}
}

namespace paso {
namespace admin {

class ActivityEditorWidget : public widget::RecordEditorWidget {
    Q_OBJECT
public:
    static QString generateRepetitionString(const QString &repetitionType,
                                            const QString &strDays);
    static QString translateScheduleType(const QString &strScheduleType);

    ActivityEditorWidget(const widget::FieldTypes &fieldTypes,
                         QWidget *parent = nullptr);

    virtual void setupUi(const QVariantMap &columnLabels,
                         const QSqlRecord &record,
                         const QStringList &filterFields = {}) override;

public slots:
    virtual void clearData() override;
    virtual void onDisplayRecord(const QSqlRecord &record) override;
    virtual void onEditExistingRecord(QSqlRecord record) override;
    virtual void onEditNewRecord(QSqlRecord record) override;

protected slots:
    virtual void accepted() override;

protected:
    virtual void prepareEdit(QSqlRecord &record) override;
    virtual bool fieldReadOnly(const QString &key) override;

    virtual QComboBox *createComboBox(const QString &field) override;

private:
    QListView *mActivityRoomsView;
    QListView *mActivityListsView;
    QTableView *mActivitySlotsView;

    paso::model::EntityTableModel *mActivityRoomsModel;
    paso::model::EntityTableModel *mActivityListsModel;
    QSqlQueryModel *mActivitySlotsModel;
};
}
}

#endif // ACTIVITYEDITORWIDGET_H
