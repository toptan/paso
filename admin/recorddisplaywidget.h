#ifndef RECORDDISPLAYWIDGET_H
#define RECORDDISPLAYWIDGET_H

#include <QMap>
#include <QString>
#include <QLineEdit>
#include <QWidget>
#include <QSqlRecord>
#include <QDialogButtonBox>

namespace paso {
namespace admin {

class RecordDisplayWidget : public QWidget {
    Q_OBJECT
public:
    explicit RecordDisplayWidget(QWidget *parent = 0);
    virtual ~RecordDisplayWidget() {}

    void setupForRecord(const QSqlRecord &record);
signals:
    void editCanceled();

public slots:
    void onDisplayRecord(const QSqlRecord &record);
    void onEditRecord(QSqlRecord record);
    void clearData();

private:
    QSqlRecord mRecord;
    QMap<QString, QWidget *> mEditFields;
    QDialogButtonBox *mButtonBox;

    QWidget *createWidgetForField(const QSqlRecord &record, int index);

private slots:
    void rejected();
    void accepted();
};
}
}

#endif // RECORDDISPLAYWIDGET_H
