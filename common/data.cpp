#include "data.h"

#include <QDebug>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

using namespace std;

namespace paso {
namespace data {

const QString roleToString(data::SystemRole role) {
    switch (role) {
    case SystemRole::ADMINISTRATOR:
        return "ADMINISTRATOR";
    case SystemRole::MANAGER:
        return "MANAGER";
    case SystemRole::ROOM_MANAGER:
        return "ROOM_MANAGER";
    case SystemRole::SCHEDULER:
        return "SCHEDULER";
    case SystemRole::SUPER_USER:
        return "SUPER_USER";
    default:
        return "INVALID_ROLE";
    }
}

SystemRole stringToRole(const QString &role) {
    if (role == "ADMINISTRATOR") {
        return SystemRole::ADMINISTRATOR;
    } else if (role == "ROOM_MANAGER") {
        return SystemRole::ROOM_MANAGER;
    } else if (role == "MANAGER") {
        return SystemRole::MANAGER;
    } else if (role == "SCHEDULER") {
        return SystemRole::SCHEDULER;
    } else if (role == "SUPER_USER") {
        return SystemRole::SUPER_USER;
    } else {
        return SystemRole::INVALID_ROLE;
    }
}

int differenceInMonths(const QDate &startDate, const QDate &endDate) {
    if (endDate < startDate) {
        return 0;
    }
    return (endDate.year() - startDate.year()) * 12 + endDate.month() -
           startDate.month();
}

list<QDateTime> scheduledDates(const QString &cronString,
                               const QDate &startDate, const QDate &endDate) {
    list<QDateTime> retVal;

    if (startDate > endDate) {
        return retVal;
    }

    auto temp = cronString.trimmed();
    auto segments = cronString.split(" ");
    if (segments.size() != 5) {
        return retVal;
    }
    auto sMinutes = segments[0].trimmed();
    auto sHours = segments[1].trimmed();
    auto sDaysOfMonth = segments[2].trimmed();
    auto sMonths = segments[3].trimmed();
    auto sDays = segments[4].trimmed();

    // We check months first.
//    auto everyMonth = sMonths == "*";
//    list<int> months;
//    if (!everyMonth) {
//        QRegularExpression monthsRegEx("((\\d{1,2}-\\d{1,2})|(\\d{1,2}))(,(("
//                                       "\\d{1,2}-\\d{1,2})|(\\d{1,2})))*");
//        auto monthsPrimaryMatch = monthsRegEx.match(sMonths);
//        if (!monthsPrimaryMatch.hasMatch() || monthsPrimaryMatch.captured() != sMonths) {
//            return retVal;
//        }

//   }

    // Checking minutes part.
    QRegularExpression minutesRegEx("^[0-9]{1,2}");
    auto match = minutesRegEx.match(sMinutes);
    if (!match.hasMatch() || match.captured() != sMinutes) {
        return retVal;
    }

    if (sMinutes.toInt() > 59) {
        return retVal;
    }

    int minutes = sMinutes.toInt();

    // Checking hours part.
    QRegularExpression hoursRegEx("^[0-9]{1,2}");
    auto hoursSplit = sHours.split(",");
    list<int> hours;
    for (const auto &h : hoursSplit) {
        auto hoursMatch = hoursRegEx.match(h);
        if (!hoursMatch.hasMatch() || hoursMatch.captured() != h) {
            return retVal;
        }
        if (h.toInt() > 23) {
            return retVal;
        }
        hours.push_back(h.toInt());
    }

    auto everyDayOfMonth = sDaysOfMonth == "*";
    list<int> daysOfMonth;
    if (!everyDayOfMonth) {
        QRegularExpression daysPrimaryRegEx("((\\d{1,2}-\\d{1,2})|(\\d{1,2}))(,"
                                            "((\\d{1,2}-\\d{1,2})|(\\d{1,2})))"
                                            "*");
        auto daysPrimaryMatch = daysPrimaryRegEx.match(sDaysOfMonth);
        if (!daysPrimaryMatch.hasMatch() ||
            daysPrimaryMatch.captured() != sDaysOfMonth) {
            return retVal;
        }

        auto commaSplit = sDaysOfMonth.split(",");
        for (const auto &cs : commaSplit) {
            auto range = cs.split("-");
            if (range.length() == 1) {
                auto day = range[0].toInt();
                if (day > 31) {
                    return retVal;
                }
                daysOfMonth.push_back(day);
            } else {
                auto r0 = range[0].toInt();
                auto r1 = range[1].toInt();

                if (r0 > 31 || r1 > 31) {
                    return retVal;
                }

                auto start = r0 < r1 ? r0 : r1;
                auto stop = r1 > r0 ? r1 : r0;
                for (auto day = start; day <= stop; day++) {
                    daysOfMonth.push_back(day);
                }
            }
        }
    }

    if (!everyDayOfMonth) {
        for (auto day : daysOfMonth) {
            auto monthsDiff = differenceInMonths(startDate, endDate);
            for (auto m = 0; m <= monthsDiff; m++) {
                for (auto h : hours) {
                    QDate d = startDate.addMonths(m);
                    d.setDate(d.year(), d.month(), day);
                    if (d >= startDate && d <= endDate) {
                        retVal.emplace_back(d, QTime(h, minutes, 0));
                    }
                }
            }
        }
    } else {
        for (auto c = 0; c <= startDate.daysTo(endDate); c++) {
            for (auto h : hours) {
                retVal.emplace_back(startDate.addDays(c), QTime(h, minutes, 0));
            }
        }
    }

    return retVal;
}
}
}
