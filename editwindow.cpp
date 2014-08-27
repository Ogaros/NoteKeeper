#include "editwindow.h"

EditWindow::EditWindow( QWidget *parent) :
    QWidget(parent)
{
    this->setUI();
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void EditWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->hide();
}

void EditWindow::setUI()
{
    mainLayout = new QVBoxLayout;
    dateLabel = new QLabel("Date:");
    selectedDate = new QDateEdit();
    noteTextLabel = new QLabel("Note text:");
    noteText = new QTextEdit;
    noteSelectorLabel = new QLabel("Selected note:");
    noteSelectorComboBox = new QComboBox;

    noteSelectorLabel->hide();
    noteSelectorComboBox->hide();

    noteText->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    noteText->setPlaceholderText("Type in the note text here");
    selectedDate->setDisplayFormat("dd.MM(MMM).yyyy");

    noteAddButtons = new QDialogButtonBox(QDialogButtonBox::Ok |
                                          QDialogButtonBox::Cancel);
    connect(noteAddButtons, SIGNAL(rejected()),this, SLOT(hide()));
    connect(noteAddButtons, SIGNAL(accepted()),this, SLOT(addNote()));

    errorLabel = new QLabel;
    errorLabel->setStyleSheet("QLabel { color : red; }");

    createRepeatGroupBox();
    createNotificationGroupBox();

    mainLayout->addWidget(dateLabel);
    mainLayout->addWidget(selectedDate);
    mainLayout->addWidget(noteSelectorLabel);
    mainLayout->addWidget(noteSelectorComboBox);
    mainLayout->addWidget(noteTextLabel);
    mainLayout->addWidget(noteText);
    mainLayout->addWidget(repeatGroupBox);
    mainLayout->addWidget(notificationGroupBox);
    mainLayout->addWidget(errorLabel);
    mainLayout->addWidget(noteAddButtons);

    mainLayout->setAlignment(Qt::AlignTop);
    this->setLayout(mainLayout);
    this->setWindowIcon(QIcon(":/images/icon.tga"));
    noteText->setFocus();
}

void EditWindow::createRepeatGroupBox()
{
    repeatLayout = new QVBoxLayout;
    repeatRadioButtonGroup = new QButtonGroup;
    repeatGroupBox = new QGroupBox("Repeat this note");
    repeatWeekRadioButton =  new QRadioButton("Every week");
    repeatMonthRadioButton =  new QRadioButton("Every month");
    repeatYearRadioButton =  new QRadioButton("Every year");

    repeatRadioButtonGroup->addButton(repeatWeekRadioButton);
    repeatRadioButtonGroup->addButton(repeatMonthRadioButton);
    repeatRadioButtonGroup->addButton(repeatYearRadioButton);

    repeatLayout->addWidget(repeatWeekRadioButton);
    repeatLayout->addWidget(repeatMonthRadioButton);
    repeatLayout->addWidget(repeatYearRadioButton);

    repeatWeekRadioButton->hide();
    repeatMonthRadioButton->hide();
    repeatYearRadioButton->hide();

    repeatGroupBox->setLayout(repeatLayout);
    repeatGroupBox->setCheckable(true);
    repeatGroupBox->setChecked(false);
    connect(repeatGroupBox, SIGNAL(toggled(const bool)), this, SLOT(showRepeatGroupBoxContent(const bool)));
}

void EditWindow::createNotificationGroupBox()
{
    notificationLayout = new QVBoxLayout;
    notificationGroupBox = new QGroupBox("Notify me in adavnce");
    notificationTopLabel = new QLabel("Starting:");
    notificationTodayRadioButton = new QRadioButton("Today");
    notificationDaysRadioButton = new QRadioButton("Amount of days prior to date");
    notificationLineEdit = new QLineEdit;
    notificationRadioButtonGroup = new QButtonGroup;
    QRegExpValidator *daysValidator = new QRegExpValidator(QRegExp("\\d+"));

    notificationLayout->addWidget(notificationTopLabel);
    notificationLayout->addWidget(notificationTodayRadioButton);
    notificationLayout->addWidget(notificationDaysRadioButton);
    notificationLayout->addWidget(notificationLineEdit);

    notificationRadioButtonGroup->addButton(notificationTodayRadioButton);
    notificationRadioButtonGroup->addButton(notificationDaysRadioButton);

    notificationLineEdit->setPlaceholderText("Amount of days");
    notificationLineEdit->setValidator(daysValidator);
    notificationLineEdit->setEnabled(false);

    notificationTopLabel->hide();
    notificationTodayRadioButton->hide();
    notificationDaysRadioButton->hide();
    notificationLineEdit->hide();

    notificationGroupBox->setLayout(notificationLayout);
    notificationGroupBox->setCheckable(true);
    notificationGroupBox->setChecked(false);
    connect(notificationGroupBox, SIGNAL(toggled(bool)), this, SLOT(showNotificationGroupBoxContent(bool)));
    connect(notificationDaysRadioButton, SIGNAL(toggled(bool)), notificationLineEdit, SLOT(setEnabled(bool)));
    connect(notificationLineEdit, SIGNAL(textChanged(QString)), this, SLOT(notificationDaysTextChange(QString)));
}

void EditWindow::changeDate(const QDate& date)
{
    selectedDate->setDate(date);
}

void EditWindow::showRepeatGroupBoxContent(const bool on)
{
    repeatWeekRadioButton->setVisible(on);
    repeatMonthRadioButton->setVisible(on);
    repeatYearRadioButton->setVisible(on);
    QTimer::singleShot(1, this, SLOT(resizeMe()));
}

void EditWindow::showNotificationGroupBoxContent(const bool on)
{
    notificationTopLabel->setVisible(on);
    notificationTodayRadioButton->setVisible(on);
    notificationDaysRadioButton->setVisible(on);
    notificationLineEdit->setVisible(on);
    QTimer::singleShot(1, this, SLOT(resizeMe()));
}

void EditWindow::resizeMe()
{
    this->resize(this->minimumSizeHint());
    noteText->resize(noteText->minimumSize());
    noteText->resize(noteText->sizeHint());
}

void EditWindow::addNote()
{
    bool hasErrors = false;
    errorLabel->clear();
    QDate date = selectedDate->date();
    QString text = noteText->toPlainText();
    if(text.size() == 0)
    {
        errorLabel->setText(errorLabel->text() + "\n*Please add some text to the note.\n");
        hasErrors = true;
    }
    nFrequency frequency = nFrequency::Once;
    if(repeatGroupBox->isChecked())
    {
        if(repeatWeekRadioButton->isChecked())
        {
            frequency = nFrequency::Week;
        }
        else if(repeatMonthRadioButton->isChecked())
        {
            frequency = nFrequency::Month;
        }
        else if(repeatYearRadioButton->isChecked())
        {
            frequency = nFrequency::Year;
        }
        else
        {
            errorLabel->setText(errorLabel->text() + "\n*You should pick when to repeat this note.\n");
            hasErrors = true;
        }
    }
    bool notifEnabled = false;
    int daysPrior = 0;
    if(notificationGroupBox->isChecked())
    {
        notifEnabled = true;
        if(!notificationTodayRadioButton->isChecked() && !notificationDaysRadioButton->isChecked())
        {
            errorLabel->setText(errorLabel->text() + "\n*You should select when to start notifying you\n about this note.\n");
            hasErrors = true;
        }
        else if(notificationDaysRadioButton->isChecked())
        {
            if(!notificationLineEdit->hasAcceptableInput())
            {
                errorLabel->setText(errorLabel->text() + "\n*You should add the number of days for notification.\n");
                hasErrors = true;
            }
            else
            {
                daysPrior = notificationLineEdit->text().toInt();
            }
        }

    }
    if(hasErrors)
    {
        errorLabel->setText(errorLabel->text().trimmed());
        errorLabel->show();
        QTimer::singleShot(1, this, SLOT(resizeMe()));
    }
    else
    {
        Note *n = currentNote;
        bool isNew = (n == nullptr);
        if(isNew)
        {
            n = new Note();
        }
        n->date = date;
        n->text = text;
        n->frequency = frequency;
        n->notifEnabled = notifEnabled;
        n->daysPrior = daysPrior;
        emit noteAdded(n, isNew);
        emit noteAdded(n->date);
        this->hide();
    }
}

void EditWindow::loadNotes(const QDate& date, std::unique_ptr<QList<Note*>> list)
{
    currentDate = date;
    noteList.reset(list.release());
    noteSelectorLabel->hide();
    noteSelectorComboBox->hide();
    if(noteList == nullptr)
    {
        loadFields();
    }
    else if(noteList->size() == 1)
    {
        loadFields(0);
    }
    else
    {
        const int lineSize = 30;
        QString textFinal;
        QString textTrimmed;
        noteSelectorLabel->show();
        noteSelectorComboBox->clear();
        noteSelectorComboBox->show();
        for(auto note : *noteList)
        {
            textTrimmed = note->text.trimmed();
            textFinal = textTrimmed.section('\n', 0, 0);
            if(textFinal.size() < textTrimmed.size() || textFinal.size() > lineSize)
            {
                textFinal = textFinal.left(lineSize) + "...";
            }
            noteSelectorComboBox->addItem(textFinal);
            connect(noteSelectorComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(loadFields(int)));
        }
        noteSelectorComboBox->currentIndexChanged(0);
    }
}

void EditWindow::loadFields(int index)
{
    errorLabel->hide();
    if(index < 0)
    {
        currentNote = nullptr;
        this->setWindowTitle("Add a new note");
        selectedDate->setDate(currentDate);
        noteText->clear();
        repeatGroupBox->setChecked(false);
        repeatRadioButtonGroup->setExclusive(false);
        repeatWeekRadioButton->setChecked(false);
        repeatMonthRadioButton->setChecked(false);
        repeatYearRadioButton->setChecked(false);
        repeatRadioButtonGroup->setExclusive(true);
        notificationGroupBox->setChecked(false);
        notificationRadioButtonGroup->setExclusive(false);
        notificationTodayRadioButton->setChecked(false);
        notificationDaysRadioButton->setChecked(false);
        notificationRadioButtonGroup->setExclusive(true);
        notificationLineEdit->clear();
    }
    else
    {
        currentNote = noteList->at(index);
        this->setWindowTitle("Edit note");
        selectedDate->setDate(currentDate);
        noteText->setText(currentNote->text);
        if(currentNote->frequency != nFrequency::Once)
        {
            repeatGroupBox->setChecked(true);
            switch(currentNote->frequency)
            {
            case nFrequency::Week:
                repeatWeekRadioButton->setChecked(true);
                break;
            case nFrequency::Month:
                repeatMonthRadioButton->setChecked(true);
                break;
            case nFrequency::Year:
                repeatYearRadioButton->setChecked(true);
                break;
            }
        }
        else
        {
            repeatGroupBox->setChecked(false);
            repeatRadioButtonGroup->setExclusive(false);
            repeatWeekRadioButton->setChecked(false);
            repeatMonthRadioButton->setChecked(false);
            repeatYearRadioButton->setChecked(false);
            repeatRadioButtonGroup->setExclusive(true);
        }
        if(currentNote->notifEnabled)
        {
            notificationGroupBox->setChecked(true);
            if(currentNote->daysPrior == 0)
            {
                notificationTodayRadioButton->setChecked(true);
                notificationLineEdit->clear();
            }
            else
            {
                notificationDaysRadioButton->setChecked(true);
                notificationLineEdit->setText(QString::number(currentNote->daysPrior));
            }
        }
        else
        {
            notificationGroupBox->setChecked(false);
            notificationRadioButtonGroup->setExclusive(false);
            notificationTodayRadioButton->setChecked(false);
            notificationDaysRadioButton->setChecked(false);
            notificationRadioButtonGroup->setExclusive(true);
            notificationLineEdit->clear();
        }
        QTimer::singleShot(1, this, SLOT(resizeMe()));
    }
}

void EditWindow::notificationDaysTextChange(const QString days)
{
    if(days.size() == 0)
    {
        notificationDaysRadioButton->setText("Amount of days prior to date");
    }
    else
    {
        notificationDaysRadioButton->setText(days + " days prior to date " + selectedDate->date().addDays(-days.toInt()).toString("(dd/MM/yyyy)"));
    }
}
