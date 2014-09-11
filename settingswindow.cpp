#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(const std::weak_ptr<Settings> settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    this->settings = settings.lock();
    ui->setupUi(this);
    QRegExpValidator *dateValidator = new QRegExpValidator(QRegExp("((d{0,4}|M{0,4}|y{2}|y{4})[/\\s\\.\\:\\\\])*"));
    ui->dateFormatEdit->setValidator(dateValidator);
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
    if(ui->repeatedShowAllRB->isChecked())
        settings->rDisplay = Settings::All;
    else if(ui->repeatedShowFutureRB->isChecked())
        settings->rDisplay = Settings::Future;
    else
        settings->rDisplay = Settings::Closest;
    if(settings->dateFormat != ui->dateFormatEdit->text())
    {
        settings->dateFormat = ui->dateFormatEdit->text();
        emit dateFormatChanged();
    }
    settings->showNumber = ui->showNumberCB->isChecked();
    settings->save();
}
