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
    explicit PDateEdit(QWidget *parent = nullptr);
    explicit PDateEdit(const QDate &date, QWidget *parent = nullptr);

protected:
    virtual QString textFromDateTime(const QDateTime &dt) const override;
};
}
}
#endif // PDATEEDIT_H
