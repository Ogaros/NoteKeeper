#include "mainwindow.h"

static const int windowSizeX = 650;
static const int windowSizeY = 300;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), isChanged(false), isClosing(false), editWindow(nullptr)
{
    try
    {
        settings = std::make_shared<Settings>();
        settings->load();
        notes = std::make_shared<Notebook>(settings);
    }
    catch(std::exception& e)
    {
        QMessageBox::critical(parent, "MainWindow::MainWindow", QString::fromLocal8Bit(e.what()));
    }
    this->setUI();
    createEditWindow();
    createSettingsWindow();
    showTrayMessage();
}

void MainWindow::showEditWindow(Note *note)
{
    const QDate d = cal->selectedDate();
    if(QObject::sender() == addButton)
    {
        editWindow->loadNotes(d, nullptr);
    }
    else if(QObject::sender() == editButton)
    {
        editWindow->loadNotes(d, notes->getNotesFromDate(d));
    }
    else //Edit button in NoteListWindow
    {
        editWindow->loadNote(note);
    }
    editWindow->show();
    editWindow->activateWindow();
    MainWindow::moveToCenter(editWindow.get());
}

void MainWindow::createEditWindow()
{
    editWindow.reset(new EditWindow(settings));
    editWindow->setWindowFlags(editWindow->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    editWindow->setWindowModality(Qt::ApplicationModal);
    connect(editWindow.get(), SIGNAL(noteAdded(Note*, const bool)), this, SLOT(addNote(Note*, const bool)));
    connect(editWindow.get(), SIGNAL(noteAdded(const QDate&)), cal, SLOT(setSelectedDate(const QDate&)));
    connect(editWindow.get(), SIGNAL(noteAdded(const QDate&)), this, SLOT(showNotes()));
    connect(editWindow.get(), SIGNAL(noteAdded(const QDate&)), this, SLOT(showClosestNote()));
    connect(editWindow.get(), SIGNAL(noteAdded(const QDate&)), this, SLOT(switchButtons()));
}

void MainWindow::createSettingsWindow()
{
    settingsWindow.reset(new SettingsWindow(settings));
    settingsWindow->setWindowFlags(settingsWindow->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    settingsWindow->hide();
    connect(settingsWindow.get(), SIGNAL(dateFormatChanged()), this, SLOT(showNotes()));
    connect(settingsWindow.get(), SIGNAL(dateFormatChanged()), this, SLOT(showClosestNote()));
    connect(settingsWindow.get(), SIGNAL(dateFormatChanged()), editWindow.get(), SLOT(refreshDateFormat()));
    connect(settingsWindow.get(), SIGNAL(rDisplayChanged()), this, SLOT(showNotes()));
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
    this->setWindowIcon(QIcon(":/images/icon.tga"));
    connect(this, SIGNAL(noteDeleted()), this, SLOT(switchButtons()));
}

void MainWindow::createMenu()
{
    QMenuBar *menu = this->menuBar();
    QMenu *fileMenu = menu->addMenu("File");
    QMenu *helpMenu = menu->addMenu("Help");

    QList<QAction *> fileActions;
    fileActions.append(new QAction("Save notes",this));
    fileActions.back()->setShortcut(Qt::CTRL | Qt::Key_S);
    fileActions.append(new QAction("Show all notes...",this));
    fileActions.append(new QAction("Delete outdated notes",this));
    fileActions.append(new QAction("Delete all notes",this));
    QAction *settings = new QAction("Settings...", this);

    connect(fileActions.front(), SIGNAL(triggered()), this, SLOT(saveNotes()));
    connect(fileActions.at(1), SIGNAL(triggered()), this, SLOT(showAllNotesWindow()));
    connect(fileActions.at(2), SIGNAL(triggered()), this, SLOT(deleteOutdated()));
    connect(fileActions.at(3), SIGNAL(triggered()), this, SLOT(deleteAll()));
    connect(settings, SIGNAL(triggered()), this, SLOT(showSettings()));

    fileMenu->addActions(fileActions);
    fileMenu->addSeparator();
    fileMenu->addAction(settings);

    QAction *aboutAction = new QAction("About...",this);
    helpMenu->addAction(aboutAction);

    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));

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

    // TODO: Make real icon.
    trayIcon->setIcon(QIcon(":/images/icon.tga"));
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

    connect(addButton, SIGNAL(clicked()), this, SLOT(showEditWindow()));
    connect(editButton, SIGNAL(clicked()), this, SLOT(showEditWindow()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(deleteNoteDialogue()));
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

void MainWindow::moveToCenter(QWidget *window)
{
    QDesktopWidget *d = QApplication::desktop();
    int ww = window->width();
    int wh = window->height();
    int dw = d->width();
    int dh = d->height();
    window->move(dw/2 - ww/2, dh/2 - wh/2);
}

void MainWindow::addNote(Note *n, const bool isNew)
{
    if(isNew)
    {
        try
        {
            notes->addNote(n);
        }
        catch(std::exception& e)
        {
            QMessageBox::critical(this, "Notebook::addNote", QString::fromLocal8Bit(e.what()));
        }
    }
    else
    {
        try
        {
            notes->sort();
        }
        catch(std::exception& e)
        {
            QMessageBox::critical(this, "Notebook::sort", QString::fromLocal8Bit(e.what()));
            qApp->quit();
        }
    }
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
                //TODO: fix line size hack
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
        QMessageBox::critical(this, "Notebook::loadNotes", QString::fromLocal8Bit(e.what()));
    }
    isChanged = false;
}

void MainWindow::deleteNoteDialogue()
{
    auto list = notes->getNotesFromDate(cal->selectedDate());
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
        QStringList listTexts;
        for(auto note : *list)
        {
            QString textTrimmed = note->text.trimmed();
            QString textCropped = textTrimmed.left(50);
            if(textTrimmed.size() > textCropped.size())
            {
                textCropped += "...";
            }
            listTexts.append(textCropped);
        }
        std::unique_ptr<DeleteDialogue> dialogue(new DeleteDialogue(listTexts));
        dialogue->setWindowFlags(dialogue->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
        moveToCenter(dialogue.get());
        dialogue->show();
        QEventLoop loop;
        connect(dialogue.get(), SIGNAL(ready()), &loop, SLOT(quit()));
        loop.exec();
        int index = dialogue->getIndex();
        if(index >= 0)
        {
            deleteNote(list->at(index));
        }
    }
    cal->setFocus();
}

void MainWindow::deleteNoteFromListWindow(Note *note)
{
    if(showDeleteMessageBox(DeleteOption::One) == QMessageBox::Yes)
    {
        deleteNote(note);
    }
}

void MainWindow::deleteNote(Note * note)
{
    notes->deleteNote(note);
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
    QMessageBox mb(this);
    mb.setWindowTitle(title);
    mb.setText(text);
    mb.setIcon(QMessageBox::Question);
    mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    return mb.exec();
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
        if(editWindow->isVisible())
        {          
            editWindow->close();
        }
        if(settingsWindow->isVisible())
        {
            settingsWindow->close();
        }
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

void MainWindow::showSettings()
{
    settingsWindow->show();
    settingsWindow->loadSettings();
}

void MainWindow::showAbout()
{
    QMessageBox about(this);
    about.setWindowTitle("About Note Keeper");
    about.setText("Do you want to save your changes before quitting?");
    about.setInformativeText("If you don't save your changes, they will be discarded.");
    about.setIcon(QMessageBox::Information);
    about.setStandardButtons(QMessageBox::Close);
    about.exec();
}

void MainWindow::showAllNotesWindow()
{
    NoteListWindow *window = new NoteListWindow(notes, settings);
    window->setAttribute(Qt::WA_DeleteOnClose);
    connect(window, SIGNAL(editNote(Note*)), this, SLOT(showEditWindow(Note*)));
    connect(window, SIGNAL(deleteNote(Note*)), this, SLOT(deleteNote(Note*)));
    connect(this, SIGNAL(noteDeleted()), window, SLOT(deleteItem()));
    connect(editWindow.get(), SIGNAL(noteEdited()), window, SLOT(updateItem()));
    window->show();
}
