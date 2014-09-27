#include "mainwindow.h"

static const int windowSizeX = 650;
static const int windowSizeY = 300;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), isChanged(false), isClosing(false)
{
    try
    {
        settings = std::make_shared<Settings>();
        settings->load();
        notes = std::make_shared<Notebook>(settings);
    }
    catch(std::exception& e)
    {
        QMessageBox::critical(parent, "MainWindow::MainWindow", e.what());
    }
    this->setUI();
    showTrayMessage();
}

void MainWindow::openEditWindow(Note *note)
{
    EditWindow *window = new EditWindow(settings);
    window->setWindowFlags(window->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    window->setWindowModality(Qt::ApplicationModal);
    window->setAttribute(Qt::WA_DeleteOnClose);
    connect(window, SIGNAL(noteAdded(Note*const)), this, SLOT(addNote(Note*const)));
    connect(window, SIGNAL(noteAdded(Note*const)), this, SLOT(showNotes()));
    connect(window, SIGNAL(noteAdded(Note*const)), this, SLOT(showClosestNote()));
    connect(window, SIGNAL(noteAdded(Note*const)), this, SLOT(switchButtons()));
    connect(window, SIGNAL(noteEdited()), this, SIGNAL(noteEdited()));
    const QDate d = cal->selectedDate();
    if(QObject::sender() == addButton)
    {
        window->loadNotes(d, nullptr);
    }
    else if(QObject::sender() == editButton)
    {
        window->loadNotes(d, notes->getNotesFromDate(d));
    }
    else //Edit button in NoteListWindow
    {
        window->loadNote(note);
    }
    window->show();
    moveToCenter(window);
}

void MainWindow::setUI()
{
    mainLayout = new QHBoxLayout;
    leftLayout = new QVBoxLayout;

    this->createMenu();
    this->createStatusBar();
    this->createCalendar();
    this->createButtonLayout();
    this->createTrayIcon();
    this->createScrollArea();
    this->setWindowTitle("Note Keeper");

    noteTextTitle = new QLabel("No notes on "+cal->selectedDate().toString(settings->dateFormat));

    leftLayout->addWidget(cal);
    leftLayout->addWidget(noteTextTitle);
    leftLayout->addWidget(scrollArea);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(buttonsLayout);

    QWidget *window = new QWidget;
    window->setLayout(mainLayout);
    this->setCentralWidget(window);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setFixedSize(this->size());
    this->setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    MainWindow::moveToCenter(this);
    this->setMinimumSize(windowSizeX,windowSizeY);
    this->setMaximumSize(windowSizeX,windowSizeY*2);
    this->setStyleSheet("QStatusBar::item { border: 0px solid black }; ");
    this->switchButtons();
    this->showNotes();
    this->showClosestNote();
    this->setWindowIcon(QIcon(":/images/icon"));
    connect(this, SIGNAL(noteDeleted()), this, SLOT(switchButtons()));
}

void MainWindow::createMenu()
{
    QMenuBar *menu = this->menuBar();
    QMenu *fileMenu = menu->addMenu("File");
    QMenu *helpMenu = menu->addMenu("Help");

    QList<QAction *> fileActions;
    fileActions.append(new QAction("Save",this));
    fileActions.back()->setShortcut(Qt::CTRL | Qt::Key_S);
    fileActions.append(new QAction("Show all notes...",this));
    fileActions.append(new QAction("Delete outdated notes",this));
    fileActions.append(new QAction("Delete all notes",this));
    QAction *settings = new QAction("Settings...", this);

    connect(fileActions.front(), SIGNAL(triggered()), this, SLOT(saveNotes()));
    connect(fileActions.at(1), SIGNAL(triggered()), this, SLOT(openAllNotesWindow()));
    connect(fileActions.at(2), SIGNAL(triggered()), this, SLOT(deleteOutdated()));
    connect(fileActions.at(3), SIGNAL(triggered()), this, SLOT(deleteAll()));
    connect(settings, SIGNAL(triggered()), this, SLOT(openSettingsWindow()));

    fileMenu->addActions(fileActions);
    fileMenu->addSeparator();
    fileMenu->addAction(settings);

    QAction *aboutAction = new QAction("About...",this);
    helpMenu->addAction(aboutAction);

    connect(aboutAction, SIGNAL(triggered()), this, SLOT(openAboutWindow()));

    connect(this, SIGNAL(noteDeleted()), this, SLOT(showClosestNote()));

    menu->show();
}

void MainWindow::createStatusBar()
{
    QStatusBar *sBar = this->statusBar();
    statusLabel = new QLabel("No noted dates in the future.");
    sBar->setSizeGripEnabled(false);
    sBar->addWidget(statusLabel);
    sBar->setSizeGripEnabled(false);
}

void MainWindow::createTrayIcon()
{
    notificationsAction = new QAction("Show today's notifications", this);
    openAction = new QAction("Open Note Keeper", this);
    quitAction = new QAction("Exit", this);

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(notificationsAction);
    trayIconMenu->addAction(openAction);
    trayIconMenu->addAction(quitAction);

    connect(notificationsAction, SIGNAL(triggered()), this, SLOT(showTrayMessage()));
    connect(openAction, SIGNAL(triggered()), this, SLOT(showFromTray()));
    openAction->setEnabled(false);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(closeProgram()));

    trayIcon = new QSystemTrayIcon(this);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTip("Note Keeper");

    trayTimer = new QTimer(this);
    trayTimer->setInterval(10000);
    trayTimer->setSingleShot(true);
    connect(trayTimer, SIGNAL(timeout()), this, SLOT(showTrayMessage()));
    connect(trayIcon, SIGNAL(messageClicked()), this, SLOT(showTrayMessage()));

    trayIcon->setIcon(QIcon(":/images/icon"));
    trayIcon->show();
}

void MainWindow::createButtonLayout()
{
    buttonsLayout = new QVBoxLayout();
    addButton = new QPushButton("Add note");
    editButton = new QPushButton("Edit note");
    removeButton = new QPushButton("Delete note");
    todayButton = new QPushButton("Go to current date");
    quitButton = new QPushButton("Close");

    connect(addButton, SIGNAL(clicked()), this, SLOT(openEditWindow()));
    connect(editButton, SIGNAL(clicked()), this, SLOT(openEditWindow()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(openDeleteDialogue()));
    connect(todayButton, SIGNAL(clicked()), cal, SLOT(showToday()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(editButton);
    buttonsLayout->addWidget(removeButton);
    buttonsLayout->addWidget(todayButton);
    buttonsLayout->addWidget(quitButton);
    buttonsLayout->setAlignment(Qt::AlignTop);
}

void MainWindow::createScrollArea()
{
    scrollArea = new QScrollArea;
    QWidget *scrollWidget = new QWidget;
    scrollLayout = new QVBoxLayout;
    scrollLayout->setMargin(5);
    scrollLayout->setAlignment(Qt::AlignTop);
    scrollWidget->setLayout(scrollLayout);
    scrollArea->setWidget(scrollWidget);
    scrollArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    scrollArea->setFixedWidth(cal->minimumWidth());
    scrollArea->setMaximumHeight(500);
    scrollArea->setMinimumHeight(0);
    scrollArea->setFixedHeight(200);
    scrollArea->setWidgetResizable(true);    
    scrollArea->hide();
}

void MainWindow::createCalendar()
{
    cal = new Calendar(notes, settings);
    cal->setMinimumSize(500, 250);
    cal->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(cal, SIGNAL(selectionChanged()), this, SLOT(switchButtons()));
    connect(cal, SIGNAL(selectionChanged()), this, SLOT(showNotes()));

}

void MainWindow::moveToCenter(QWidget * const window)
{
    QDesktopWidget *d = QApplication::desktop();
    int ww = window->width();
    int wh = window->height();
    int dw = d->width();
    int dh = d->height();
    window->move(dw/2 - ww/2, dh/2 - wh/2);
}

void MainWindow::addNote(Note * const note)
{
    notes->addNote(note);
    isChanged = true;
}

void MainWindow::showNotes()
{
    const QDate d = cal->selectedDate();
    scrollArea->hide();
    auto notesList = notes->getNotesFromDate(d);
    const int notesCount = notesList->size();
    int labelsSize = noteLabels.size();
    if(notesCount != 0)
    {
        noteTextTitle->setText(d.toString(settings->dateFormat + ":"));
        while(notesCount > labelsSize)
        {
            addNoteLabel();
            ++labelsSize;
        }
        for(int i = 0; i < labelsSize; i++)
        {
            if(i < notesCount)
            {
                noteLabels[i]->setText(notesList->at(i)->text);
                noteLabels[i]->setEnabled(true);
                noteLabels[i]->show();
            }
            else
            {
                noteLabels[i]->setEnabled(false);
                noteLabels[i]->hide();
            }
        }
    }
    else
    {
        noteTextTitle->setText("No notes on "+d.toString(settings->dateFormat));
    }
    cal->setFocus();
    resizeTimer();
}

void MainWindow::addNoteLabel()
{
    QLabel *label = new QLabel;
    label->setFrameShape(QFrame::StyledPanel);
    label->setContentsMargins(2, 0, 0, 2);
    label->setMargin(5);
    scrollLayout->addWidget(label);
    noteLabels.append(label);
}

void MainWindow::switchButtons()
{
    if(notes->contains(cal->selectedDate()))
    {
        editButton->show();
        removeButton->show();
    }
    else
    {
        editButton->hide();
        removeButton->hide();
    }
}

void MainWindow::resizeMe()
{
    if(noteTextTitle->text().left(8) != "No notes")
    {
        int scrollAreaHeight = 0;
        for(auto label : noteLabels)
        {
            if(label->isEnabled())
            {
                label->adjustSize();
                label->setFixedWidth(scrollArea->widget()->width() - scrollLayout->margin() * 2 - 20);
                scrollAreaHeight += label->frameGeometry().height() + label->margin();
            }
        }
        scrollAreaHeight += scrollLayout->margin() + 5;
        if(scrollAreaHeight > cal->minimumHeight())
        {
            scrollAreaHeight = cal->minimumHeight();
        }
        scrollArea->setFixedHeight(scrollAreaHeight);
        scrollArea->show();
    }
    this->resize(this->minimumSizeHint());
}

void MainWindow::resizeTimer()
{
    QTimer::singleShot(1, this, SLOT(resizeMe()));
}

void MainWindow::saveNotes()
{
    try
    {
        notes->saveNotes();
    }
    catch(std::exception& e)
    {
        QMessageBox::critical(this, "Notebook::saveNotes", QString::fromLocal8Bit(e.what()));
    }
    isChanged = false;
}

void MainWindow::openDeleteDialogue()
{    
    auto list = std::move(notes->getNotesFromDate(cal->selectedDate()));
    if(list->size() == 1)
    {
        int i = showDeleteMessageBox(DeleteOption::One);
        if(i == QMessageBox::Yes)
        {
            deleteNote(list->first());
        }
    }
    else
    {        
        DeleteDialogue *dialogue = new DeleteDialogue(std::move(list));
        dialogue->setAttribute(Qt::WA_DeleteOnClose);
        connect(dialogue, SIGNAL(deleteNotes(std::shared_ptr<QList<Note*>>)), this, SLOT(deleteNotes(std::shared_ptr<QList<Note*>>)));
        dialogue->show();
        moveToCenter(dialogue);
    }
    cal->setFocus();
}

void MainWindow::deleteNoteFromListWindow(Note * const note)
{
    if(showDeleteMessageBox(DeleteOption::One) == QMessageBox::Yes)
    {
        deleteNote(note);
    }
}

void MainWindow::deleteNote(Note * const note)
{
    notes->deleteNote(note);
    this->showNotes();
    isChanged = true;
    emit noteDeleted();
}

void MainWindow::deleteNotes(std::shared_ptr<QList<Note *>> noteList)
{
    for(auto note : *noteList)
    {
        notes->deleteNote(note);
    }
    this->showNotes();
    isChanged = true;
    emit noteDeleted();
}

void MainWindow::deleteAll()
{
    int i = showDeleteMessageBox(DeleteOption::All);
    if(i == QMessageBox::Yes)
    {
        if(notes->deleteAll() > 0)
            isChanged = true;
        this->showNotes();
        emit noteDeleted();
    }
    cal->setFocus();
}

void MainWindow::deleteOutdated()
{
    int i = showDeleteMessageBox(DeleteOption::Outdated);
    if(i == QMessageBox::Yes)
    {
        if(notes->deleteOutdated(QDate::currentDate()) > 0)
            isChanged = true;
        this->showNotes();
        emit noteDeleted();
    }
    cal->setFocus();
}

int MainWindow::showDeleteMessageBox(const DeleteOption op)
{
    QString title, text;
    switch(op)
    {
    case DeleteOption::One:
        title = "Delete note?";
        text = "Are you sure you want to delete this note?";
        break;
    case DeleteOption::Outdated:
        title = "Delete outdated notes?";
        text = "Are you sure you want to delete all outdated notes?";
        break;
    case DeleteOption::All:
        title = "Delete all notes?";
        text = "Are you sure you want to delete all notes?";
        break;
    default:
        break;
    }
    return QMessageBox::question(this, title, text, QMessageBox::Yes, QMessageBox::No);
}
int MainWindow::showExitWOSavingMessageBox()
{
    QMessageBox mb(this);
    mb.setWindowTitle("Some notes have been changed");
    mb.setText("Do you want to save your changes before quitting?");
    mb.setInformativeText("If you don't save your changes, they will be discarded.");
    mb.setIcon(QMessageBox::Warning);
    mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    return mb.exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(isClosing)
    {
        if(isChanged)
        {
            int i = showExitWOSavingMessageBox();
            switch(i)
            {
            case QMessageBox::Yes:
                saveNotes();
                event->accept();
                qApp->quit();
                break;
            case QMessageBox::No:
                event->accept();
                qApp->quit();
                break;
            case QMessageBox::Cancel:
                isClosing = false;
                event->ignore();
                this->showFromTray();
                break;
            default:
                break;
            }
        }
        else
        {
            event->accept();
            qApp->quit();
        }
    }
    else
    {
        event->ignore();
        this->hide();
        openAction->setEnabled(true);
    }
}

void MainWindow::showClosestNote()
{
    int noteTextPieceSize = 25;
    Note *n = notes->findClosest(QDate::currentDate());
    if(n != nullptr)
    {
        QString statusText = "Closest noted date" + n->date.toString("("+settings->dateFormat+"):");
        QString noteTextTrimmed = n->text.trimmed();
        QString noteTextLine = noteTextTrimmed.section('\n',0,0);
        if(noteTextLine.size() > noteTextPieceSize || noteTextTrimmed.size() > noteTextLine.size())
        {
            statusText += noteTextLine.left(noteTextPieceSize) + "...";
        }
        else
        {
            statusText += noteTextLine;
        }
        statusLabel->setText(statusText);
    }
    else
        statusLabel->setText("No noted dates in the future.");
}

void MainWindow::closeProgram()
{
    isClosing = true;
    this->close();
}

void MainWindow::showFromTray()
{
    this->show();
    this->activateWindow();
    cal->setFocus();
    openAction->setEnabled(false);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:
        if(!this->isVisible())
        {
            this->showFromTray();
        }
        break;
    default:
        break;
    }
}

void MainWindow::showTrayMessage()
{
    static int noteIndex = -1;
    static std::unique_ptr<QList<Note*>> noteList;
    trayTimer->stop();
    if(noteIndex < 0)
    {
        noteList.reset(notes->getNotificationsFromDate(QDate::currentDate()).release());
        if(!noteList->isEmpty())
        {
            noteIndex = 0;
        }
        else
        {
            if(QObject::sender() == notificationsAction)
                trayIcon->showMessage("Note Keeper", "No notifications today", QSystemTrayIcon::Information, trayTimer->interval());
        }
    }
    if(noteIndex >= 0)
    {
        if(noteIndex < noteList->size())
        {
            Note* note = noteList->at(noteIndex);
            QString Title = note->date.toString(settings->dateFormat);
            qint64 days = QDate::currentDate().daysTo(note->date);
            switch(days)
            {
            case 0:
                Title += " (Today)    ";
                break;
            case 1:
                Title += " (in "+QString::number(days)+" day):";
                break;
            default:
                Title += " (in "+QString::number(days)+" days):";
                break;
            }
            trayIcon->showMessage(Title, note->text, QSystemTrayIcon::Information, trayTimer->interval());
            trayTimer->start();
            ++noteIndex;
        }
        else
        {
            trayIcon->hide();
            trayIcon->show();
            noteIndex = -1;
        }
    }
}

void MainWindow::openSettingsWindow()
{
    SettingsWindow *window = new SettingsWindow(settings);
    window->setWindowFlags(window->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    window->setWindowModality(Qt::ApplicationModal);
    window->setAttribute(Qt::WA_DeleteOnClose);
    connect(window, SIGNAL(dateFormatChanged()), this, SLOT(showNotes()));
    connect(window, SIGNAL(dateFormatChanged()), this, SLOT(showClosestNote()));
    connect(window, SIGNAL(rDisplayChanged()), this, SLOT(showNotes()));
    window->loadSettings();
    window->show();
}

void MainWindow::openAboutWindow()
{
    QWidget *about = new QWidget;
    QLabel *icon = new QLabel;
    icon->setPixmap(QIcon(":/images/icon").pixmap(64,64));
    QLabel *text = new QLabel("Note Keeper ver. 1.0\n\nMade by Fedor Rebenkov");
    QFont font;
    font.setPointSize(10);
    text->setFont(font);
    QPushButton *close = new QPushButton("Close");
    QVBoxLayout *layout = new QVBoxLayout;
    QHBoxLayout *textIcon = new QHBoxLayout;
    textIcon->addWidget(icon);
    textIcon->addWidget(text);
    layout->addLayout(textIcon);
    layout->addWidget(close,0,Qt::AlignRight);
    connect(close, SIGNAL(clicked()), about, SLOT(close()));
    about->setLayout(layout);
    about->setWindowTitle("About Note Keeper");
    about->setAttribute(Qt::WA_DeleteOnClose);
    about->setWindowFlags(about->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    about->setWindowModality(Qt::ApplicationModal);
    about->show();
    int aw = about->width();
    int ah = about->height();
    int ww = this->width();
    int wh = this->height();
    about->move(ww/2 - aw/2 + this->x(), wh/2 - ah/2 + this->y());
}

void MainWindow::openAllNotesWindow()
{
    NoteListWindow *window = new NoteListWindow(notes, settings);
    window->setAttribute(Qt::WA_DeleteOnClose);
    connect(window, SIGNAL(editNote(Note*const)), this, SLOT(openEditWindow(Note*const)));
    connect(window, SIGNAL(deleteNote(Note*const)), this, SLOT(deleteNote(Note*const)));
    connect(this, SIGNAL(noteDeleted()), window, SLOT(deleteItem()));
    connect(this, SIGNAL(noteEdited()), window, SLOT(updateItem()));
    window->show();
}
