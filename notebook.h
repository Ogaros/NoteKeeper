#ifndef NOTEBOOK_H
#define NOTEBOOK_H
#include "widgetList.h"
#include "note.h"
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

class Notebook
{
public:
    //TODO: remove parent from here
    Notebook(QWidget *parent);
    ~Notebook();
    void loadNotes();
    void saveNotes() const;
    bool addNote(Note *n);
    bool addNote(const QDate nDate, const QString nText);
    bool addNote(const QDate nDate, const QString nText,
                 const QDate notifDate, const QTime notifTime, const bool isRepeated);
    bool deleteNote(Note *n);
    int deleteOutdated(const QDate&);
    int deleteAll();
    QString* getTextFromDate(const QDate&) const;
    Note* getNoteFromDate(const QDate&) const;
    Note* findClosest(const QDate&) const;
    bool contains (const QDate&) const;
    void sort();


private:
    QWidget *parent;
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
