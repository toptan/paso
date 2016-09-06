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

class RoomEditorWidget : public widget::RecordEditorWidget {
    Q_OBJECT

public:
    RoomEditorWidget(const widget::FieldTypes &fieldTypes,
                     QWidget *parent = nullptr);

    virtual void setupUi(const QVariantMap &columnLabels,
                         const QSqlRecord &record,
                         const QStringList &filterFields = {}) override;

public slots:
    void onChangeBarringButtonClicked();

    virtual void clearData() override;
    virtual void onDisplayRecord(const QSqlRecord &record) override;

protected:
    virtual void prepareEdit(QSqlRecord &record) override;
    virtual bool fieldReadOnly(const QString &key) override;

    virtual QLineEdit *createLineEdit(const QString &field) override;

protected slots:
    virtual void accepted() override;

private:
    paso::db::DBManager mManager;
    QTableView *mBarredStudentsTableView;
    QPushButton *mChangeBarringButton;

    paso::model::EntityTableModel *mBarredStudentsModel;
};
}
}

#endif // ROOMEDITORWIDGET_H
