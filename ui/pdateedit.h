#ifndef PDATEEDIT_H
#define PDATEEDIT_H

#include <QDate>
#include <QDateEdit>
#include <QString>

namespace paso {
namespace widget {

///
/// \brief The PDateEdit class is simple extension of QDateEdit that is capable
/// of displaying empty text. If minimum date is set the QDateEdit will display
/// it when passed null or invalid date, we want empty string.
///
class PDateEdit : public QDateEdit {
    Q_OBJECT
public:
    ///
    /// \brief Contructs widget with given parent.
    /// \param parent The parent widget.
    ///
    explicit PDateEdit(QWidget *parent = nullptr);

    ///
    /// \brief Constucts widget with given date and parent.
    /// \param date The date to se initially.
    /// \param parent The parent widget.
    ///
    explicit PDateEdit(const QDate &date, QWidget *parent = nullptr);

protected:
    ///
    /// \brief Overriden function to return empty string if invalid date time is
    /// set.
    /// \param dt The date and time object.
    /// \return String to display for given date time object.
    ///
    virtual QString textFromDateTime(const QDateTime &dt) const override;
};
}
}
#endif // PDATEEDIT_H
