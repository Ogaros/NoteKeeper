#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include "widgetList.h"
#include "note.h"
#include "QRegExpValidator"
#include "memory"
#include "settings.h"

class EditWindow : public QWidget
{
    Q_OBJECT
public:
    explicit EditWindow(std::weak_ptr<Settings>, QWidget *parent = 0);
    void loadNotes(const QDate&, std::unique_ptr<QList<Note*>>);
    bool isOpened(){return this->isVisible();}

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void noteAdded(Note*, const bool);
    void noteAdded(const QDate&);

public slots:    
    void changeDate(const QDate& date);
    void resizeMe();
    void addNote();

private slots:
    void loadFields(int index = -1);
    void notificationDaysTextChange(const QString);
    void showRepeatGroupBoxContent(const bool on);
    void showNotificationGroupBoxContent(const bool on);
    void refreshDateFormat();

private:
    void setUI();
    void createRepeatGroupBox();
    void createNotificationGroupBox();
    void fillNoteSelector();    
    void resizeTimer();

    std::shared_ptr<Settings>       settings;
    Note                *currentNote;
    QDate               currentDate;
    QVBoxLayout         *mainLayout;
    QLabel              *dateLabel;
    QDateEdit           *selectedDate;
    QLabel              *noteSelectorLabel;
    QComboBox           *noteSelectorComboBox;
    QLabel              *noteTextLabel;    
    QTextEdit           *noteText;
    QGroupBox           *repeatGroupBox;
    QVBoxLayout         *repeatLayout;
    QButtonGroup        *repeatRadioButtonGroup;
    QRadioButton        *repeatWeekRadioButton;
    QRadioButton        *repeatMonthRadioButton;
    QRadioButton        *repeatYearRadioButton;
    QGroupBox           *notificationGroupBox;
    QVBoxLayout         *notificationLayout;
    QLineEdit           *notificationLineEdit;
    QLabel              *notificationTopLabel;
    QRadioButton        *notificationTodayRadioButton;
    QRadioButton        *notificationDaysRadioButton;
    QButtonGroup        *notificationRadioButtonGroup;
    QLabel              *errorLabel;
    QDialogButtonBox    *noteAddButtons;
    std::unique_ptr<QList<Note*>> noteList;
};

#endif // EDITWINDOW_H
