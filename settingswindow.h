#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include "QCloseEvent"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
