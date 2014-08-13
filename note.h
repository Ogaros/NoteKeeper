#ifndef NOTE_H
#define NOTE_H
#include "widgetList.h"

class Note
{
public:
    Note(){}
    Note(const QDate nDate, const QString nText);
    Note(const QDate nDate, const QString nText, const QDate notifDate,
         const QTime notifTime, const bool isRepeated);

    bool operator <(const Note& other) const
    {
        return this->nDate < other.nDate;
    }

    QString nText;
    QDate nDate, notifDate;
    QTime notifTime;
    bool isNotified, isRepeated;
};

#endif // NOTE_H
