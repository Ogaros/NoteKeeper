#include "note.h"

Note::Note(const QDate date, const QString text, const nFrequency frequency,
           const bool notifEnabled, const int daysPrior):
           date(date), text(text), frequency(frequency), notifEnabled(notifEnabled), daysPrior(daysPrior){}
