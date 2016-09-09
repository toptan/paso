#ifndef ENTRYREPORTDIALOG_H
#define ENTRYREPORTDIALOG_H

#include <QDate>
#include <QDialog>

class QSqlTableModel;

namespace Ui {
class EntryReportDialog;
}

namespace paso {
namespace admin {

class EntryReportDialog : public QDialog {
    Q_OBJECT
public:
    explicit EntryReportDialog(QWidget *parent = 0);
    virtual ~EntryReportDialog();

    void setRoomNumber(const QString &roomNumber);

    void setPersonNumber(const QString &personNumber);

private slots:
    void fromDateChanged(const QDate &newDate);
    void toDateChanged(const QDate &newDate);
    void onExportButtonClicked();

private:
    enum Mode { Undefined, Room, Person };

    Ui::EntryReportDialog *ui;

    Mode mMode;
    QString mFilter;
    QDate mFrom;
    QDate mTo;
    QSqlTableModel *mTableModel;

    void refreshData();
};
}
}

#endif // ENTRYREPORTDIALOG_H
