#ifndef NOTE_H
#define NOTE_H
#include "widgetList.h"

enum class nFrequency {Once, Week, Month, Year};
class Note
{
public:
    Note(){}
    Note(const QDate date, const QString text, const nFrequency frequency,
         const bool notifEnabled, const QDate startDate);

    bool operator <(const Note& other) const
    {
        return this->date < other.date;
    }

    QDate date;
    QString text;
    nFrequency frequency;
    bool notifEnabled;
    QDate startDate;
};

#endif // NOTE_H
