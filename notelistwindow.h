#ifndef NOTELISTWINDOW_H
#define NOTELISTWINDOW_H

#include <QWidget>
#include "notebook.h"
#include "memory"
#include "settings.h"


namespace Ui {
class NoteListWindow;
}

class NoteListWindow : public QWidget
{
    Q_OBJECT

public:
    explicit NoteListWindow(const std::weak_ptr<Notebook>, const std::weak_ptr<Settings>, QWidget *parent = 0);
    ~NoteListWindow();

private:
    void setConnections();
    void setupList();
    Ui::NoteListWindow *ui;
    std::shared_ptr<Notebook> notes;
    std::shared_ptr<Settings> settings;
    std::map<int, Note*> indexMap;

private slots:
    void edit();
    void remove();

signals:
    void editNote(Note *);
    void deleteNote(Note *);
    void addNote();
};

#endif // NOTELISTWINDOW_H
