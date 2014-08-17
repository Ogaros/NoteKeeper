#include "mainwindow.h"

static const int windowSizeX = 650;
static const int windowSizeY = 300;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), isChanged(false), isClosing(false), editWindow(nullptr)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFixedSize(this->size());
    notes = new Notebook(this);
    this->setUI();
    setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    MainWindow::moveToCenter(this);
    showNote();
    showClosestNote();
    connect(this, SIGNAL(noteDeleted()), this, SLOT(switchButtons()));
}

void MainWindow::showAddWindow()
{
    const QDate d = cal->selectedDate();
    if(editWindow == nullptr)
    {
        editWindow = new EditWindow();
        editWindow->loadFields(d, notes->getNoteFromDate(d));
        connect(editWindow, SIGNAL(noteAdded(Note*, const bool)), this, SLOT(addNote(Note*, const bool)));
        connect(editWindow, SIGNAL(noteAdded(const QDate&)), cal, SLOT(setSelectedDate(const QDate&)));
        connect(editWindow, SIGNAL(noteAdded(const QDate&)), this, SLOT(showNote()));
        connect(editWindow, SIGNAL(noteAdded(const QDate&)), this, SLOT(showClosestNote()));
        connect(editWindow, SIGNAL(noteAdded(const QDate&)), this, SLOT(switchButtons()));
        editWindow->show();
        MainWindow::moveToCenter(editWindow);
    }
    else
    {
        editWindow->loadFields(d, notes->getNoteFromDate(d));
        QTimer::singleShot(1, editWindow, SLOT(resizeMe()));
        editWindow->show();
    }
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

    noteTextTitle = new QLabel("No notes on "+cal->selectedDate().toString("dddd dd of MMMM yyyy"));
    noteText = new QLabel();
    noteText->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    noteText->setMargin(5);
    noteText->hide();

//    scrollArea = new QScrollArea;
//    scrollArea->setWidget(noteText);
//    scrollArea->setMaximumSize(cal->size());
//    scrollArea->setMinimumSize(cal->size());
//    noteText->setMinimumSize(100,100);

    leftLayout->addWidget(cal);
    leftLayout->addWidget(noteTextTitle);
    leftLayout->addWidget(noteText);

    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(buttonsLayout);

    QWidget *window = new QWidget;
    window->setLayout(mainLayout);
    this->setCentralWidget(window);
    this->setMinimumSize(windowSizeX,windowSizeY);
    this->setMaximumSize(windowSizeX,windowSizeY*2);
    this->setStyleSheet("QStatusBar::item { border: 0px solid black }; ");
    this->switchButtons();
}

void MainWindow::createMenu()
{
    QMenuBar *menu = this->menuBar();
    QMenu *fileMenu = menu->addMenu("File");
    QMenu *helpMenu = menu->addMenu("Help");

    QList<QAction *> fileActions;
    fileActions.append(new QAction("Save notes",this));
    fileActions.append(new QAction("Import notes...",this));
    fileActions.append(new QAction("Export notes...",this));
    fileActions.append(new QAction("Delete outdated notes",this));
    fileActions.append(new QAction("Delete all notes",this));

    connect(fileActions.front(), SIGNAL(triggered()), this, SLOT(saveNotes()));
    connect(fileActions.at(3), SIGNAL(triggered()), this, SLOT(deleteOutdated()));
    connect(fileActions.at(4), SIGNAL(triggered()), this, SLOT(deleteAll()));

    fileMenu->addActions(fileActions);

    helpMenu->addAction(new QAction("How to use...",this));
    helpMenu->addAction(new QAction("About...",this));

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
    openAction = new QAction("Open Organizer", this);
    quitAction = new QAction("Exit", this);

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(openAction);
    trayIconMenu->addAction(quitAction);

    connect(openAction, SIGNAL(triggered()), this, SLOT(showFromTray()));
    openAction->setEnabled(false);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(closeProgram()));

    trayIcon = new QSystemTrayIcon(this);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setToolTip("Organizer");

    // TODO: Make real icon.
    trayIcon->setIcon(QIcon(":/images/icon.tga"));
    trayIcon->show();
}

void MainWindow::createButtonLayout()
{
    buttonsLayout = new QVBoxLayout();
    addButton = new QPushButton("Add note");
    editButton = new QPushButton("Edit note");
    removeButton = new QPushButton("Remove note");
    todayButton = new QPushButton("Go to current date");
    quitButton = new QPushButton("Close");

    connect(addButton, SIGNAL(clicked()), this, SLOT(showAddWindow()));
    connect(editButton, SIGNAL(clicked()), this, SLOT(showAddWindow()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(deleteNode()));
    connect(todayButton, SIGNAL(clicked()), cal, SLOT(showToday()));
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));

    buttonsLayout->addWidget(addButton);
    buttonsLayout->addWidget(editButton);
    buttonsLayout->addWidget(removeButton);
    buttonsLayout->addWidget(todayButton);
    buttonsLayout->addWidget(quitButton);
    buttonsLayout->setAlignment(Qt::AlignTop);
}

void MainWindow::createCalendar()
{
    cal = new orgCalendar(notes);
    cal->setMinimumSize(500, 250);
    cal->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(cal, SIGNAL(selectionChanged()), this, SLOT(switchButtons()));
    connect(cal, SIGNAL(selectionChanged()), this, SLOT(showNote()));
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
        if(notes->addNote(n) == false)
        {
            QMessageBox::critical(this, "MainWindow::addNote", "Null pointer passed as an argument");
        }
    }
    else
    {
        notes->sort();
    }
    isChanged = true;
}

void MainWindow::showNote()
{
    QDate d = cal->selectedDate();
    noteText->hide();
    QString *text = notes->getTextFromDate(d);
    if(text != nullptr)
    {
        noteTextTitle->setText(d.toString("dddd dd of MMMM yyyy:"));
        noteText->setText(*text);
    }
    else
    {
        noteTextTitle->setText("No notes on "+d.toString("dddd dd of MMMM yyyy"));
    }
    cal->setFocus();

    QTimer::singleShot(1, this, SLOT(resizeMe()));
}

void MainWindow::switchButtons()
{
    if(notes->contains(cal->selectedDate()))
    {
        addButton->hide();
        editButton->show();
        removeButton->show();
    }
    else
    {
        addButton->show();
        editButton->hide();
        removeButton->hide();
    }
}

void MainWindow::resizeMe()
{
    if(noteTextTitle->text().left(8) != "No notes")
    {
        noteText->show();
    }
    this->resize(this->minimumSizeHint());
}

void MainWindow::saveNotes()
{
    notes->saveNotes();
    isChanged = false;
}

void MainWindow::deleteNode()
{
    int i = showDeleteMessageBox(DeleteOption::One);
    if(i == QMessageBox::Yes)
    {
        notes->deleteNote(notes->getNoteFromDate(cal->selectedDate()));
        this->showNote();
        isChanged = true;
        emit noteDeleted();
    }
    cal->setFocus();
}

void MainWindow::deleteAll()
{
    int i = showDeleteMessageBox(DeleteOption::All);
    if(i == QMessageBox::Yes)
    {
        if(notes->deleteAll() > 0)
            isChanged = true;
        this->showNote();
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
        this->showNote();
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
                delete notes;
                event->accept();
                qApp->quit();
                break;
            case QMessageBox::No:
                delete notes;
                event->accept();
                qApp->quit();
                break;
            case QMessageBox::Cancel:
                event->ignore();
                break;
            default:
                delete notes;
                event->accept();
                qApp->quit();
                break;
            }
        }
        else
        {
            delete notes;
            event->accept();
            qApp->quit();
        }
    }
    else
    {
        event->ignore();
        if(editWindow != nullptr)
        {
            editWindow->close();
        }
        this->hide();
        openAction->setEnabled(true);
        //Temporary
        showTrayMessage(QDate::currentDate().addDays(10));
    }
}

void MainWindow::showClosestNote()
{
    int noteTextPieceSize = 25;
    Note *n = notes->findClosest(QDate::currentDate());
    if(n != nullptr)
    {
        QString statusText = "Closest noted date" + n->nDate.toString("(dd/MM/yyyy): ");
        QString noteTextTrimmed = n->nText.trimmed();
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
    cal->setFocus();
    openAction->setEnabled(false);
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:
        if(this->isVisible())
        {
            this->close();
        }
        else
        {
            this->showFromTray();
        }
        break;
    default:
        break;
    }
}

void MainWindow::showTrayMessage(const QDate& date)
{
    trayIcon->showMessage(date.toString("dd/MM/yyyy")+" (in "+QString::number(static_cast<ulong>(QDate::currentDate().daysTo(date)))+" days):",
                          "*notes->getTextFromDate(date)");
}
