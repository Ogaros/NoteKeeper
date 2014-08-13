#include "note.h"


Note::Note(const QDate nDate, const QString nText) :
    nText(nText), nDate(nDate), isNotified(false)
{

}

Note::Note(const QDate nDate, const QString nText, const QDate notifDate,
           const QTime notifTime, const bool isRepeated) :
    nText(nText), nDate(nDate), notifDate(notifDate), notifTime(notifTime), isNotified(true),
    isRepeated(isRepeated)
{

}
