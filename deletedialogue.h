#ifndef DELETEDIALOGUE_H
#define DELETEDIALOGUE_H

#include "QWidget"
#include "memory"
#include "note.h"
#include "notetreeitem.h"

namespace Ui {
class DeleteDialogue;
}

class DeleteDialogue : public QWidget
{
    Q_OBJECT

public:
    explicit DeleteDialogue(std::unique_ptr<QList<Note*>>, QWidget *parent = 0);
    ~DeleteDialogue();

signals:
    void deleteNotes(std::shared_ptr<QList<Note*>>);

private slots:
    void sendDeleteList();

private:
    void setupList();
    void setupItem(Note * const note, QTreeWidgetItem * const item) const;
    int showConfirmation(bool isSingle);
    Ui::DeleteDialogue *ui;
    std::unique_ptr<QList<Note*>> notes;
    std::map<int, Note*> indexMap;
};

#endif // DELETEDIALOGUE_H
