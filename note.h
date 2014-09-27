#ifndef NOTE_H
#define NOTE_H

#include "widgetList.h"

enum class nFrequency {Once, Week, Month, Year};
class Note
{
public:
    Note(){}
    Note(const QDate date, const QString text, const nFrequency frequency,
         const bool notifEnabled, const int daysPrior):
         date(date), text(text), frequency(frequency), notifEnabled(notifEnabled), daysPrior(daysPrior){}

    QDate date;
    QString text;
    nFrequency frequency;
    bool notifEnabled;
    int daysPrior;
};

#endif // NOTE_H
