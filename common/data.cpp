#include "data.h"

#include <QDebug>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include <set>

using namespace std;

namespace paso {
namespace data {

const QString roleToString(SystemRole role) {
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

const QString activityTypeToString(ActivityType type) {
    switch (type) {
    case ActivityType::LECTURE:
        return "LECTURE";
    case ActivityType::EXAM:
        return "EXAM";
    case ActivityType::COLLOQUIUM:
        return "COLLOQUIUM";
    case ActivityType::LAB_EXCERCISE:
        return "LAB_EXCERCISE";
    case ActivityType::INDIVIDUAL_WORK:
        return "INDIVIDUAL_WORK";
    case ActivityType::SPECIAL_EVENT:
        return "SPECIAL_EVENT";
    default:
        return "INVALID_ACTIVITY";
    }
}

ActivityType stringToActivityType(const QString &type) {
    if (type == "LECTURE") {
        return ActivityType::LECTURE;
    } else if (type == "EXAM") {
        return ActivityType::EXAM;
    } else if (type == "COLLOQUIUM") {
        return ActivityType::COLLOQUIUM;
    } else if (type == "LAB_EXCERCISE") {
        return ActivityType::LAB_EXCERCISE;
    } else if (type == "INDIVIDUAL_WORK") {
        return ActivityType::INDIVIDUAL_WORK;
    } else if (type == "SPECIAL_EVENT") {
        return ActivityType::SPECIAL_EVENT;
    } else {
        return ActivityType::INVALID_ACTIVITY;
    }
}

const QString activityScheduleTypeToString(ActivityScheduleType type) {
    switch (type) {
    case ActivityScheduleType::ONCE:
        return "ONCE";
    case ActivityScheduleType::WEEK_DAYS:
        return "WEEK_DAYS";
    case ActivityScheduleType::MONTH_DAYS:
        return "MONTH_DAYS";
    default:
        return "INVALID_ACTIVITY";
    }
}

ActivityScheduleType stringToActivityScheduleType(const QString &type) {
    if (type == "ONCE") {
        return ActivityScheduleType::ONCE;
    } else if (type == "WEEK_DAYS") {
        return ActivityScheduleType::WEEK_DAYS;
    } else if (type == "MONTH_DAYS") {
        return ActivityScheduleType::MONTH_DAYS;
    } else {
        return ActivityScheduleType::INVALID;
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
    auto sDaysOfWeek = segments[4].trimmed();

    // Days of month and days of week cannot be specified together.
    if (sDaysOfMonth != "*" && sDaysOfWeek != "*") {
        return retVal;
    }

    // We check months first.
    auto everyMonth = sMonths == "*";
    list<int> months;
    if (!everyMonth) {
        QRegularExpression monthsRegEx("((\\d{1,2}-\\d{1,2})|(\\d{1,2}))(,(("
                                       "\\d{1,2}-\\d{1,2})|(\\d{1,2})))*");
        auto monthsPrimaryMatch = monthsRegEx.match(sMonths);
        if (!monthsPrimaryMatch.hasMatch() ||
            monthsPrimaryMatch.captured() != sMonths) {
            return retVal;
        }

        auto commaSplit = sMonths.split(",");
        for (const auto &cs : commaSplit) {
            auto range = cs.split("-");
            if (range.length() == 1) {
                auto month = range[0].toInt();
                if (month > 12) {
                    return retVal;
                }
                months.push_front(month);
            } else {
                auto r0 = range[0].toInt();
                auto r1 = range[1].toInt();

                if (r0 > 12 || r1 > 12) {
                    return retVal;
                }

                auto start = r0 < r1 ? r0 : r1;
                auto stop = r1 > r0 ? r1 : r0;
                for (auto month = start; month <= stop; month++) {
                    months.push_back(month);
                }
            }
        }
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

    // Lets finally check days of week.
    auto everyDayOfWeek = sDaysOfWeek == "*";
    list<int> daysOfWeek;
    if (!everyDayOfWeek) {
        QRegularExpression daysOfWeekRegEx("^[0-6](,[0-6]+)*");
        auto match = daysOfWeekRegEx.match(sDaysOfWeek);
        if (!match.hasMatch() || match.captured() != sDaysOfWeek) {
            return retVal;
        }
        auto split = sDaysOfWeek.split(",");
        for (const auto &s : split) {
            // Cron Sunday is 0, but QDate Sunday is 7.
            daysOfWeek.push_back(s.toInt() == 0 ? 7 : s.toInt());
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
        auto monthsDiff = differenceInMonths(startDate, endDate);
        for (auto m = 0; m <= monthsDiff; m++) {
            for (auto c = 0; c <= startDate.daysTo(endDate); c++) {
                for (auto h : hours) {
                    auto d = startDate.addMonths(m).addDays(c);
                    if (d >= startDate && d <= endDate) {
                        retVal.emplace_back(startDate.addMonths(m).addDays(c),
                                            QTime(h, minutes, 0));
                    }
                }
            }
        }
    }

    // Let's remove all dates with months that are not in cron string.
    if (!everyMonth) {
        list<QDateTime> toRemove;
        for (const auto &dateTime : retVal) {
            if (find(months.begin(), months.end(), dateTime.date().month()) ==
                    months.end() ||
                !dateTime.isValid()) {
                toRemove.push_back(dateTime);
            }
        }

        for (const auto &dt : toRemove) {
            retVal.remove(dt);
        }
    }

    // Let's remove dates which don't match given days of week.
    if (!everyDayOfWeek) {
        list<QDateTime> toRemove;
        for (const auto &dateTime : retVal) {
            if (find(daysOfWeek.begin(), daysOfWeek.end(),
                     dateTime.date().dayOfWeek()) == daysOfWeek.end()) {
                toRemove.push_back(dateTime);
            }
        }
        for (const auto &dt : toRemove) {
            retVal.remove(dt);
        }
    }

    // And finally remove duplicates if any.
    retVal.sort();
    retVal.unique();
    return retVal;
}
}
}
