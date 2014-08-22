#include "note.h"

Note::Note(const QDate date, const QString text, const nFrequency frequency,
           const bool notifEnabled, const QDate startDate):
           date(date), text(text), frequency(frequency), notifEnabled(notifEnabled), startDate(startDate){}
