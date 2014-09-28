#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include "QWidget"
#include "QCloseEvent"
#include "QAbstractButton"
#include "settings.h"
#include "memory"
#include "QRegExpValidator"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWindow(const std::weak_ptr<Settings>, QWidget *parent = 0);
    ~SettingsWindow();    
    void loadSettings();
    void loadSettings(Settings * const l_settings);

private slots:
    void buttonsClicked(QAbstractButton *button);
    void extraY(QString newFormat);

signals:
    void dateFormatChanged();
    void rDisplayChanged();

private:
    void setConnections() const;
    void saveSettings();
    int strDiff(const QString &first, const QString &second) const;
    Ui::SettingsWindow *ui;
    std::shared_ptr<Settings> settings;    
    QString prevFormat;
};

#endif // SETTINGSWINDOW_H
