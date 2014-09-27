#ifndef NOTELISTWINDOW_H
#define NOTELISTWINDOW_H

#include "QWidget"
#include "notebook.h"
#include "memory"
#include "settings.h"
#include "notetreeitem.h"


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
    void setupItem(Note *, QTreeWidgetItem *);
    Ui::NoteListWindow *ui;
    std::shared_ptr<Notebook> notes;
    std::shared_ptr<Settings> settings;
    std::map<int, Note*> indexMap;    

public slots:
    void updateItem();
    void deleteItem();

private slots:
    void edit();
    void remove();

signals:
    void editNote(Note *);
    void deleteNote(Note *);
};

#endif // NOTELISTWINDOW_H
