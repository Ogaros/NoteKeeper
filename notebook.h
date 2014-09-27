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
    friend class NoteListWindow;
public:
    Notebook(const std::weak_ptr<Settings>);
    ~Notebook();
    void loadNotes();
    void saveNotes() const;
    void addNote(Note * const note);
    bool deleteNote(Note * const note);
    int deleteOutdated(const QDate &date);
    int deleteAll();
    int contains (const QDate &date) const;
    Note* findClosest(const QDate &date) const;
    std::unique_ptr<QList<Note*>> getNotesFromDate(const QDate &date) const;
    std::unique_ptr<QList<Note*>> getNotificationsFromDate(const QDate &date) const;

private:
    const QString filePath = QApplication::applicationDirPath() + "/Notes.xml";
    QList<Note*> notes;
    std::shared_ptr<Settings> settings;
    bool noteOnDate(Note * const note, const QDate &date) const;
    bool notificationOnDate(Note * const note, const QDate &date) const;
    QDate getClosestDate(Note * const note, const QDate &date) const;
    Note* parseNote(QXmlStreamReader&) const;
};

#endif // NOTEBOOK_H
