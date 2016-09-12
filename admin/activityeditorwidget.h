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

///
/// \brief The ActivityEditorWidget class is activity editor widget. This widget
/// is always in read only mode as all activity editing goes through
/// ActivityWizard.
///
class ActivityEditorWidget : public widget::RecordEditorWidget {
    Q_OBJECT
public:
    ///
    /// \brief Generates string for activity repetition type like "Monday,
    /// Wednesday and Friday" or "Every 10, /// 15 and 20 in month"
    /// \param repetitionType The repetition type, "WEEK_DAYS" or "MONTH_DAYS"
    /// \param strDays The string containing days.
    /// \return The repetition string.
    ///
    static QString generateRepetitionString(const QString &repetitionType,
                                            const QString &strDays);
    ///
    /// \brief Returns translated activity schedule type.
    /// \param strScheduleType The activity schedule type string, "ONCE",
    /// "WEEK_DAYS" or "MONTH_DAYS".
    /// \return Translated activity schedule type.
    ///
    static QString translateScheduleType(const QString &strScheduleType);

    ///
    /// \brief Constructs activity editor widget for given field types.
    /// \param fieldTypes The record field types.
    /// \param parent The parent widget.
    ///
    ActivityEditorWidget(const widget::FieldTypes &fieldTypes,
                         QWidget *parent = nullptr);

    /// \copydoc widget::RecordEditorWidget::setupUi()
    virtual void setupUi(const QVariantMap &columnLabels,
                         const QSqlRecord &record,
                         const QStringList &filterFields = {}) override;

public slots:
    /// \copydoc widget::RecordEditorWidget::clearData()
    virtual void clearData() override;
    /// \copydoc widget::RecordEditorWidget::onDisplayRecord()
    virtual void onDisplayRecord(const QSqlRecord &record) override;
    /// \copydoc widget::RecordEditorWidget::onEditExistingRecord()
    virtual void onEditExistingRecord(QSqlRecord record) override;
    /// \copydoc widget::RecordEditorWidget::onEditNewRecord()
    virtual void onEditNewRecord(QSqlRecord record) override;

protected slots:
    /// \copydoc widget::RecordEditorWidget::accepted()
    virtual void accepted() override;

protected:
    /// \copydoc widget::RecordEditorWidget::prepareEdit()
    virtual void prepareEdit(QSqlRecord &record) override;
    /// \copydoc widget::RecordEditorWidget::fieldReadOnly()
    virtual bool fieldReadOnly(const QString &key) override;
    /// \copydoc widget::RecordEditorWidget::createComboBox()
    virtual QComboBox *createComboBox(const QString &field) override;

private:
    QListView *mActivityRoomsView;  //!< View to display activity rooms.
    QListView *mActivityListsView;  //!< View to display activity lists.
    QTableView *mActivitySlotsView; //!< View to display activity time slots.

    /// Model that holds data about activity rooms.
    paso::model::EntityTableModel *mActivityRoomsModel;
    /// Model that holds data about activity lists.
    paso::model::EntityTableModel *mActivityListsModel;
    /// Model that holds data about activity time slots.
    QSqlQueryModel *mActivitySlotsModel;
};
}
}

#endif // ACTIVITYEDITORWIDGET_H
