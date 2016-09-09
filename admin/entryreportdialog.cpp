#include "entryreportdialog.h"
#include "ui_entryreportdialog.h"

#include "pasodb.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlTableModel>

using namespace paso::db;

namespace paso {
namespace admin {

EntryReportDialog::EntryReportDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::EntryReportDialog), mMode(Undefined) {
    ui->setupUi(this);
    mTableModel =
        new QSqlTableModel(this, QSqlDatabase::database(DEFAULT_DB_NAME));
    mTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    ui->tableView->setModel(mTableModel);
    mTableModel->setTable("room_entry_data");

    mFrom = QDate::currentDate().addMonths(-1);
    mTo = QDate::currentDate();
    ui->fromDate->setDate(mFrom);
    ui->toDate->setDate(mTo);
}

EntryReportDialog::~EntryReportDialog() { delete ui; }

void EntryReportDialog::fromDateChanged(const QDate &newDate) {
    mFrom = newDate;
    refreshData();
}

void EntryReportDialog::toDateChanged(const QDate &newDate) {
    mTo = newDate;
    refreshData();
}

void EntryReportDialog::onExportButtonClicked() {
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Select file to export room entries"), QString(), QString(),
        nullptr, QFileDialog::DontUseNativeDialog);
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Error"));
        msgBox.setText(
            tr("The export file '%1' could not be created.").arg(fileName));
        msgBox.setDetailedText(file.errorString());
        msgBox.exec();
        return;
    }

    QTextStream out(&file);
    auto columnCount = mTableModel->columnCount();
    for (auto i = 0; i < mTableModel->rowCount(); i++) {
        const QSqlRecord &record = mTableModel->record(i);
        QString line;
        for (auto j = 0; j < columnCount - 1; j++) {
            line += record.value(j).toString() + ",";
        }
        line +=
            record.value(columnCount - 1).toDateTime().toString(Qt::ISODate);
        out << line << "\n";
    }
    out.flush();
    file.close();
    QMessageBox::information(
        this, tr("Information"),
        tr("Exporting room entries to file '%1' has finished.").arg(fileName));
}

void EntryReportDialog::setRoomNumber(const QString &roomNumber) {
    mMode = Room;
    mFilter = roomNumber;
    refreshData();
}

void EntryReportDialog::setPersonNumber(const QString &personNumber) {
    mMode = Person;
    mFilter = personNumber;
    refreshData();
}

void EntryReportDialog::refreshData() {
    auto filterStr =
        QString("entry_time::date >= '%1' AND entry_time::date <= '%2' ")
            .arg(mFrom.toString(Qt::ISODate))
            .arg(mTo.toString(Qt::ISODate));
    if (mMode == Room) {
        filterStr += QString("AND room_number ='%1'").arg(mFilter);
    } else if (mMode == Person) {
        filterStr += QString("AND person_number = '%1'").arg(mFilter);
    }
    mTableModel->setFilter(filterStr);
    mTableModel->select();
    mTableModel->setHeaderData(0, Qt::Horizontal, tr("Room Number"));
    mTableModel->setHeaderData(1, Qt::Horizontal, tr("Ind. or Empl. Nr."));
    mTableModel->setHeaderData(2, Qt::Horizontal, tr("First Name"));
    mTableModel->setHeaderData(3, Qt::Horizontal, tr("Last Name"));
    mTableModel->setHeaderData(4, Qt::Horizontal, tr("Entry Time"));
}
}
}
