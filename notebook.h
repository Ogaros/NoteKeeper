#ifndef NOTEBOOK_H
#define NOTEBOOK_H
#include "widgetList.h"
#include "note.h"
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include "memory"

class Notebook
{
public:
    Notebook();
    Notebook(Notebook*){loadNotes();}
    ~Notebook();
    void loadNotes();
    void saveNotes() const;
    bool addNote(Note *n);
    bool addNote(const QDate date, const QString text, const nFrequency frequency,
                 const bool notifEnabled, const QDate startDate);
    bool deleteNote(Note *n);
    int deleteOutdated(const QDate&);
    int deleteAll();
    QString* getTextFromDate(const QDate&) const;
    std::unique_ptr<QList<Note*>> getNotesFromDate(const QDate&) const;
    Note* findClosest(const QDate&) const;
    bool contains (const QDate&) const;
    void sort();


private:
    bool noteOnDate(Note* const, const QDate&) const;
    QList<Note*> notes;
    Note* parseNote(QXmlStreamReader&) const;    
};

struct ptrLess
{
    bool operator()(const Note *a, const Note *b)
    {
        if(a != nullptr && b != nullptr)
        {
            return *a < *b;
        }
        else
        {
            throw std::exception("Null pointer passed as an argument");
        }
    }
};

#endif // NOTEBOOK_H
