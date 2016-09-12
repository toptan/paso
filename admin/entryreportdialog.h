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

///
/// \brief The EntryReportDialog class is a dialog that displays room entries.
/// It has two modes of operation, "room" and "person". When in "room" mode it
/// will display entries to the room of any person in given date range. In
/// "person" mode it displays entries of selected person into any room for
/// selected period.
///
class EntryReportDialog : public QDialog {
    Q_OBJECT
public:
    ///
    /// \brief Constructs dialog with given parent.
    /// \param parent The parent widget
    ///
    explicit EntryReportDialog(QWidget *parent = 0);
    ///
    /// \brief Destructor.
    ///
    virtual ~EntryReportDialog();

    ///
    /// \brief Sets the room number and switches mode to display room entries
    /// for any person.
    /// \param roomNumber The room number.
    ///
    void setRoomNumber(const QString &roomNumber);

    ///
    /// \brief Sets person's identification number and switches mode to display
    /// person entries to any room. The person's identification number is index
    /// number for students and employee number for teachers.
    /// \param personNumber Person's identification number.
    ///
    void setPersonNumber(const QString &personNumber);

private slots:
    ///
    /// \brief Called when new date is selected from the "from" date picker.
    /// \param newDate The new date.
    ///
    void fromDateChanged(const QDate &newDate);
    ///
    /// \brief Called when new date is selected from the "to" date picker.
    /// \param newDate The new date.
    ///
    void toDateChanged(const QDate &newDate);
    ///
    /// \brief This slot is called by clicking on the export button to perform
    /// data export to CSV file.
    ///
    void onExportButtonClicked();

private:
    ///
    /// \brief The Mode enum defines mode of operation for the dialog.
    ///
    enum Mode {
        Undefined, //!< The mode is undefined or no yet set.
        Room,      //!< Display enrty data for room.
        Person     //!< Display entry data for person.
    };

    Ui::EntryReportDialog *ui; //!< The generated form object.

    Mode mMode;      //!< The mode of operation.
    QString mFilter; //!< The filter part of the SQL query.
    QDate mFrom;     //!< The date from which to display room entries.
    QDate mTo;       //!< The date to which to display room entries.
    QSqlTableModel *mTableModel; //!< The underlying data table model.

    ///
    /// \brief Refreshes currently displayed data by reexecuting query and also
    /// called whenever data range changes.
    ///
    void refreshData();
};
}
}

#endif // ENTRYREPORTDIALOG_H
