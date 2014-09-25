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
    ui->treeWidget->sortItems(0, Qt::AscendingOrder);
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

void NoteListWindow::setupItem(Note *note, QTreeWidgetItem *item)
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
    if(item != nullptr)
        emit editNote(indexMap[item->text(10).toInt()]);
}

void NoteListWindow::remove()
{
    ui->treeWidget->setFocus();
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    if(item != nullptr)
        emit deleteNote(indexMap[item->text(10).toInt()]);
}

NoteTreeItem::NoteTreeItem(const std::weak_ptr<Settings> settings)
{
    this->settings = settings.lock();
    frequencyMap.emplace("no", 0);
    frequencyMap.emplace("weekly", 1);
    frequencyMap.emplace("monthly", 2);
    frequencyMap.emplace("yearly", 3);
}

bool NoteTreeItem::operator <(const QTreeWidgetItem &other) const
{
    int column = treeWidget()->sortColumn();
    QString thisText = text(column);
    QString otherText = other.text(column);
    switch(column)
    {
    case 0: //Date
        return QDate::fromString(thisText, settings->dateFormat) < QDate::fromString(otherText, settings->dateFormat);
    case 1: //Text
        return thisText < otherText;
    case 2: //Frequency
        return frequencyMap.at(thisText) < frequencyMap.at(otherText);
    case 3: //Notification
        {
            if(thisText == "no" && otherText != "no")
                return true;
            else if(otherText == "no")
                return false;
            else
            {
                QString aaa = thisText.section(' ',0,0);
                QString bbb = otherText.section(' ',0,0);
                return aaa.toInt() < bbb.toInt();
            }
        }
    default:
        throw std::logic_error("sorting non-existing row");
    }
}
