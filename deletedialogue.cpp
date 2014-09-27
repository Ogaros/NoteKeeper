#include "deletedialogue.h"
#include "ui_deletedialogue.h"

DeleteDialogue::DeleteDialogue(std::unique_ptr<QList<Note*>> notes, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeleteDialogue)
{
    ui->setupUi(this);
    ui->noteList->header()->setSectionsMovable(false);
    this->notes = std::move(notes);
    setupList();
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(sendDeleteList()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

DeleteDialogue::~DeleteDialogue()
{
    delete ui;
}

void DeleteDialogue::setupList()
{
    QTreeWidget *list = ui->noteList;
    int index = 0;
    for(auto note : *notes)
    {
        std::shared_ptr<Settings> settings;
        NoteTreeItem *item = new NoteTreeItem(settings);
        setupItem(note, item);
        item->setText(10, QString::number(index));
        list->addTopLevelItem(item);
        indexMap.emplace(index++, note);
    }
    list->sortItems(10, Qt::AscendingOrder);
}

void DeleteDialogue::setupItem(Note *note, QTreeWidgetItem *item)
{
    item->setText(0, note->text);
    QString frequency;
    switch(note->frequency)
    {
    case nFrequency::Once:
        frequency = "no";
        break;
    case nFrequency::Week:
        frequency = "weekly";
        break;
    case nFrequency::Month:
        frequency = "monthly";
        break;
    case nFrequency::Year:
        frequency = "yearly";
        break;
    }
    item->setText(1, frequency);
    QString notification;
    if(note->notifEnabled)
    {
        notification = QString::number(note->daysPrior);
        notification += note->daysPrior == 1 ? " day prior" : " days prior";
    }
    else
        notification = "no";
    item->setText(2, notification);
}

void DeleteDialogue::sendDeleteList()
{
    auto itemList = ui->noteList->selectedItems();    
    int size = itemList.size();
    if(size == 0)
    {
        QMessageBox::warning(this, "No notes selected", "Select one or more notes to delete", QMessageBox::Ok);
        ui->noteList->setFocus();
    }
    else if(showConfirmation(size == 1) == QMessageBox::Yes)
    {
        QList<Note*> *noteList = new QList<Note*>;
        for(auto item : itemList)
        {
            noteList->append(indexMap[item->text(10).toInt()]);
        }
        emit deleteNotes(std::shared_ptr<QList<Note*>>(noteList));
        close();
    }
    else
        ui->noteList->setFocus();
}

int DeleteDialogue::showConfirmation(bool isSingle)
{
    QString title, text;
    if(isSingle)
    {
        title = "Delete note?";
        text = "Are you sure you want to delete selected note?";
    }
    else
    {
        title = "Delete notes?";
        text = "Are you sure you want to delete selected notes?";
    }
    return QMessageBox::question(this, title, text, QMessageBox::Yes, QMessageBox::No);
}
