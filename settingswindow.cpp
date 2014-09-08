#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(std::weak_ptr<Settings> settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    this->settings = settings.lock();
    ui->setupUi(this);
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

void SettingsWindow::setConnections()
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
        settings->restore();
        loadSettings();
        break;
    }
}

void SettingsWindow::loadSettings()
{
    ui->dateFormatEdit->setText(settings->dateFormat);
    ui->startupCheckBox->setChecked(settings->autorun);
    switch(settings->rDisplay)
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
}

void SettingsWindow::saveSettings()
{
    settings->autorun = ui->startupCheckBox->isChecked();
    settings->dateFormat = ui->dateFormatEdit->text();
    if(ui->repeatedShowAllRB->isChecked())
        settings->rDisplay = Settings::All;
    else if(ui->repeatedShowFutureRB->isChecked())
        settings->rDisplay = Settings::Future;
    else
        settings->rDisplay = Settings::Closest;
    settings->save();
}
