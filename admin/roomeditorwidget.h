#ifndef ROOMEDITORWIDGET_H
#define ROOMEDITORWIDGET_H

#include "recordeditorwidget.h"

class QTableView;
class QSqlQueryModel;
class QPushButton;

namespace paso {
namespace model {
class EntityTableModel;
}
}

namespace paso {
namespace admin {

///
/// \brief The RoomEditorWidget class is room editor widget.
///
class RoomEditorWidget : public widget::RecordEditorWidget {
    Q_OBJECT

public:
    ///
    /// \brief Constructs room editor widget for given field types.
    /// \param fieldTypes The record field types.
    /// \param parent The parent widget.
    ///
    RoomEditorWidget(const widget::FieldTypes &fieldTypes,
                     QWidget *parent = nullptr);

    /// \copydoc widget::RecordEditorWidget::setupUi()
    virtual void setupUi(const QVariantMap &columnLabels,
                         const QSqlRecord &record,
                         const QStringList &filterFields = {}) override;

public slots:
    ///
    /// \brief The slot that shows dialog for editing barred students.
    ///
    void onChangeBarringButtonClicked();
    ///
    /// \brief The slot that initiates room occupancy report export.
    ///
    void onExportOccupancyClicked();

    /// \copydoc widget::RecordEditorWidget::clearData()
    virtual void clearData() override;
    /// \copydoc widget::RecordEditorWidget::onDisplayRecord()
    virtual void onDisplayRecord(const QSqlRecord &record) override;
    /// \copydoc widget::RecordEditorWidget::onEditExistingRecord()
    virtual void onEditExistingRecord(QSqlRecord record) override;
    /// \copydoc widget::RecordEditorWidget::onEditNewRecord()
    virtual void onEditNewRecord(QSqlRecord record) override;

protected:
    /// \copydoc widget::RecordEditorWidget::prepareEdit()
    virtual void prepareEdit(QSqlRecord &record) override;
    /// \copydoc widget::RecordEditorWidget::fieldReadOnly()
    virtual bool fieldReadOnly(const QString &key) override;
    /// \copydoc widget::RecordEditorWidget::createLineEdit()
    virtual QLineEdit *createLineEdit(const QString &field) override;

protected slots:
    /// \copydoc widget::RecordEditorWidget::accepted()
    virtual void accepted() override;

private:
    paso::db::DBManager mManager; //!< The database manager to use.

    /// Table view that displays barred students.
    QTableView *mBarredStudentsTableView;
    /// Table view that displays occupancy data.
    QTableView *mOccupancyTableView;
    /// The model that holds occupancy data.
    QSqlQueryModel *mOccupancyTableModel;

    /// The button to show dialog for editing barred students.
    QPushButton *mChangeBarringButton;
    /// The button to export room occupancy report.
    QPushButton *mExportOccupancy;

    /// The model that holds data about barred students.
    paso::model::EntityTableModel *mBarredStudentsModel;
    /// The id of room being edited, or \c 0 if it is a new room.
    quint64 mRoomId;
};
}
}

#endif // ROOMEDITORWIDGET_H
