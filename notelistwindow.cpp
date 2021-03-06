#include "notelistwindow.h"
#include "ui_notelistwindow.h"

NoteListWindow::NoteListWindow(const std::weak_ptr<Notebook> notes, const std::weak_ptr<Settings> settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoteListWindow)
{
    this->notes = notes.lock();
    this->settings = settings.lock();
    ui->setupUi(this);
    ui->treeWidget->header()->setSectionsMovable(false);
    setupList();
    try
    {
        ui->treeWidget->sortItems(0, Qt::AscendingOrder);        
    }
    catch(std::exception &e)
    {
        QMessageBox::critical(this, "NoteListWindow::NoteListWindow", e.what(), QMessageBox::Close);
        qApp->quit();
    }
    ui->treeWidget->resizeColumnToContents(1);
    ui->treeWidget->setStyleSheet("QTreeWidget {alternate-background-color: #dfdfdf;}");
    this->resize(this->sizeHint().width(), this->height());
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
    int index = 0;
    for(auto note : notes->notes)
    {
        NoteTreeItem *item = new NoteTreeItem(settings);
        setupItem(note, item);
        item->setText(10, QString::number(index));
        list->addTopLevelItem(item);
        indexMap.emplace(index++, note);
    }
}

void NoteListWindow::updateItem()
{
    QTreeWidgetItem *item = ui->treeWidget->selectedItems().first();
    setupItem(indexMap[item->text(10).toInt()], item);
}

void NoteListWindow::deleteItem()
{
    int index = ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->currentItem());
    delete ui->treeWidget->takeTopLevelItem(index);
}

void NoteListWindow::setupItem(Note * const note, QTreeWidgetItem * const item) const
{
    item->setText(0, note->date.toString(settings->dateFormat));
    item->setText(1, note->text);
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
    item->setText(2, frequency);
    QString notification;
    if(note->notifEnabled)
    {
        notification = QString::number(note->daysPrior);
        notification += note->daysPrior == 1 ? " day prior" : " days prior";
    }
    else
        notification = "no";
    item->setText(3, notification);
}

void NoteListWindow::edit()
{
    ui->treeWidget->setFocus();
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    if(ui->treeWidget->selectedItems().size() == 0)
    {
        QMessageBox::warning(this, "No notes selected", "Select a note to edit", QMessageBox::Ok);
        ui->treeWidget->setFocus();
    }
    else
        emit editNote(indexMap[item->text(10).toInt()]);
}

void NoteListWindow::remove()
{
    ui->treeWidget->setFocus();
    QTreeWidgetItem *item = ui->treeWidget->currentItem();    
    if(ui->treeWidget->selectedItems().size() == 0)
    {
        QMessageBox::warning(this, "No notes selected", "Select a note to delete", QMessageBox::Ok);
        ui->treeWidget->setFocus();
    }
    else if(showConfirmation() == QMessageBox::Yes)
        emit deleteNote(indexMap[item->text(10).toInt()]);
    else
        ui->treeWidget->setFocus();
}

int NoteListWindow::showConfirmation()
{
    return QMessageBox::question(this, "Delete note?",  "Are you sure you want to delete selected note?", QMessageBox::Yes, QMessageBox::No);
}
