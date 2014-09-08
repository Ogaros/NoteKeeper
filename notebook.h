#ifndef NOTEBOOK_H
#define NOTEBOOK_H
#include "widgetList.h"
#include "note.h"
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include "memory"
#include "settings.h"

class Notebook
{
public:
    Notebook(const std::weak_ptr<Settings>);
    ~Notebook();
    void loadNotes();
    void saveNotes() const;
    void addNote(Note *n);
    void addNote(const QDate date, const QString text, const nFrequency frequency,
                 const bool notifEnabled, const int daysPrior);
    bool deleteNote(Note *n);
    int deleteOutdated(const QDate&);
    int deleteAll();
    std::unique_ptr<QList<Note*>> getNotesFromDate(const QDate&) const;
    std::unique_ptr<QList<Note*>> getNotificationsFromDate(const QDate&) const;
    Note* findClosest(const QDate&) const;
    bool contains (const QDate&) const;
    void sort();

private:
    const QString filePath = QApplication::applicationDirPath() + "/OrgNotes.xml";
    bool noteOnDate(Note* const, const QDate&) const;
    bool notificationOnDate(Note* const note, const QDate&date) const;
    QList<Note*> notes;
    std::shared_ptr<Settings> settings;
    Note* parseNote(QXmlStreamReader&) const;

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
                throw std::runtime_error("Null pointer passed as an argument");
            }
        }
    };
};

#endif // NOTEBOOK_H
