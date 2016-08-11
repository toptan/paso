#include "pdateedit.h"

PDateEdit::PDateEdit(QWidget *parent) : QDateEdit(parent) {}

PDateEdit::PDateEdit(const QDate &date, QWidget *parent)
    : QDateEdit(date, parent) {}

QString PDateEdit::textFromDateTime(const QDateTime &dt) const {
    if (dt.isNull() || !dt.isValid()) {
        return "";
    }
    return QDateEdit::textFromDateTime(dt);
}
