#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(const std::weak_ptr<Settings> settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    this->settings = settings.lock();
    ui->setupUi(this);
    QRegExpValidator *dateValidator = new QRegExpValidator(QRegExp("((d{0,4}|M{0,4}|y{0,4})[/\\s\\.\\,\\:\\\\])*"));
    ui->dateFormatEdit->setValidator(dateValidator);    
    ui->iconLabel->setPixmap(QIcon(":/images/qMarkIcon").pixmap(20,20));
    ui->iconLabel->setToolTip("Date format can be formedwith the following expressions:<br>"
                              "<table>"
                              "<tr><td><b>d</b></td><td>the day as number without a leading zero (1 to 31)</td></tr>"
                              "<tr><td><b>dd</b></td><td>the day as number with a leading zero (01 to 31)</td></tr>"
                              "<tr><td><b>ddd</b></td><td>the abbreviated day name ('Mon' to 'Sun')</td></tr>"
                              "<tr><td><b>dddd</b></td><td>the long day name ('Monday' to 'Sunday')</td></tr>"
                              "<tr><td><b>M</b></td><td>the month as number without a leading zero (1 to 12)</td></tr>"
                              "<tr><td><b>MM</b></td><td>the month as number with a leading zero (01 to 12)</td></tr>"
                              "<tr><td><b>MMM</b></td><td>the abbreviated month name ('Jan' to 'Dec')</td></tr>"
                              "<tr><td><b>MMMM &nbsp;</b></td><td>the long month name ('January' to 'December')</td></tr>"
                              "<tr><td><b>yy</b></td><td>the year as two digit number (00 to 99)</td></tr>"
                              "<tr><td><b>yyyy</b></td><td>the year as four digit number (1993)</td></tr>"
                              "</table>"
                              "<br><br>You can use white spaces and . , : / \\ symbols to separate"
                              "<br>the expressions.");
    setConnections();
    loadSettings();
    prevFormat = ui->dateFormatEdit->text();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::setConnections() const
{
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonsClicked(QAbstractButton*)));
    connect(ui->dateFormatEdit, SIGNAL(textEdited(QString)), this, SLOT(extraY(QString)));
}

void SettingsWindow::buttonsClicked(QAbstractButton *button)
{
    QDialogButtonBox::StandardButton sButton = ui->buttonBox->standardButton(button);
    switch(sButton)
    {
    case QDialogButtonBox::Apply:
        saveSettings();
    case QDialogButtonBox::Cancel:
        hide();
        break;
    case QDialogButtonBox::RestoreDefaults:
    {
        std::unique_ptr<Settings> temp_settings(new Settings);
        loadSettings(temp_settings.get());
        break;
    }
    }
}

void SettingsWindow::loadSettings()
{
    loadSettings(settings.get());
}

void SettingsWindow::loadSettings(Settings * const l_settings)
{
    ui->dateFormatEdit->setText(l_settings->dateFormat);
    ui->startupCheckBox->setChecked(l_settings->autorun);
    switch(l_settings->rDisplay)
    {
    case Settings::All:
        ui->repeatedShowAllRB->setChecked(true);
        break;
    case Settings::Future:
        ui->repeatedShowFutureRB->setChecked(true);
        break;
    case Settings::Closest:
        ui->repeatedShowClosestRB->setChecked(true);
        break;
    }
    ui->showNumberCB->setChecked(l_settings->showNumber);
}

void SettingsWindow::saveSettings()
{
    settings->autorun = ui->startupCheckBox->isChecked();
    Settings::repeatedDisplay rDisplay;
    if(ui->repeatedShowAllRB->isChecked())
        rDisplay = Settings::All;
    else if(ui->repeatedShowFutureRB->isChecked())
        rDisplay = Settings::Future;
    else
        rDisplay = Settings::Closest;
    if(settings->rDisplay != rDisplay)
    {
        settings->rDisplay = rDisplay;
        emit rDisplayChanged();
    }
    if(settings->dateFormat != ui->dateFormatEdit->text())
    {
        settings->dateFormat = ui->dateFormatEdit->text();
        emit dateFormatChanged();
    }
    settings->showNumber = ui->showNumberCB->isChecked();
    settings->save();
}

void SettingsWindow::extraY(QString newFormat)
{
    // Adds/removes an extra 'y' when user adds/removes 'y' to/from the date format field.
    int index = strDiff(prevFormat, newFormat);
    if(index < 0)
    {
        prevFormat = newFormat;
        return;
    }
    bool deleted;
    if(newFormat.size() >= prevFormat.size() && newFormat[index] == 'y')
        deleted = false;
    else if(newFormat.size() <= prevFormat.size() && prevFormat[index] == 'y')
        deleted = true;
    else
    {
        prevFormat = newFormat;
        return;
    }
    if(deleted)
    {
        if(index == 0 || (index < prevFormat.size()-1 && prevFormat[index+1] == 'y'))
            newFormat.remove(index, 1);
        else
            newFormat.remove(index - 1, 1);
    }
    else
    {
        newFormat.insert(index, 'y');
    }
    ui->dateFormatEdit->setText(newFormat);
    ui->dateFormatEdit->setCursorPosition(deleted ? index - 1 : index + 2);
    prevFormat = newFormat;
}

int SettingsWindow::strDiff(const QString &first, const QString &second) const
{
    // Compares 2 strings and returns an index of the first different character.
    // Returns -1 if strings are equal.
    int fSize = first.size();
    int sSize = second.size();
    int size = fSize < sSize ? fSize : sSize;
    for(int i = 0; i < size; i++)
    {
        if(first[i] != second[i])
        {
            return i;
        }
    }
    if(fSize != sSize)
        return fSize > sSize ? fSize - 1 : sSize - 1;
    return -1;
}
