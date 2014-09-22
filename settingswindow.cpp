#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(const std::weak_ptr<Settings> settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    this->settings = settings.lock();
    ui->setupUi(this);
    QRegExpValidator *dateValidator = new QRegExpValidator(QRegExp("((d{0,4}|M{0,4}|y{0,4})[/\\s\\.\\,\\:\\\\])*"));
    //TODO: add extra y when user inputs y
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
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->hide();
}

void SettingsWindow::setConnections() const
{
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonsClicked(QAbstractButton*)));
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

void SettingsWindow::loadSettings(Settings *l_settings)
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
