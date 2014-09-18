#include "notelistwindow.h"
#include "ui_notelistwindow.h"

NoteListWindow::NoteListWindow(const std::weak_ptr<Notebook> notes, const std::weak_ptr<Settings> settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoteListWindow)
{
    this->notes = notes.lock();
    this->settings = settings.lock();
    ui->setupUi(this);
    setupList();
    setConnections();
}

NoteListWindow::~NoteListWindow()
{
    delete ui;
}

void NoteListWindow::setConnections()
{
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(remove()));
    connect(ui->editButton, SIGNAL(clicked()), this, SLOT(edit()));
}

void NoteListWindow::setupList()
{
    QTreeWidget *list = ui->treeWidget;
    for(auto note : notes->notes)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(list);
        item->setText(0, note->date.toString(settings->dateFormat));
        item->setText(1, note->text);
        list->addTopLevelItem(item);
        indexMap.emplace(list->indexOfTopLevelItem(item), note);
    }
}

void NoteListWindow::edit()
{
    emit editNote(indexMap[ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->selectedItems().first())]);
}

void NoteListWindow::remove()
{
    emit deleteNote(indexMap[ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->selectedItems().first())]);
}
