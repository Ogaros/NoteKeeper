#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include "QCloseEvent"
#include "QAbstractButton"
#include "settings.h"
#include "memory"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWindow(std::weak_ptr<Settings>, QWidget *parent = 0);
    ~SettingsWindow();
    void loadSettings();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void buttonsClicked(QAbstractButton*);

signals:
    void dateFormatChanged();

private:
    void setConnections();    
    void saveSettings();    
    void loadSettings(Settings *l_settings);
    Ui::SettingsWindow *ui;
    std::shared_ptr<Settings> settings;    
};

#endif // SETTINGSWINDOW_H
