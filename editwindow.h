#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include "note.h"
#include "QRegExpValidator"
#include "memory"
#include "settings.h"

class EditWindow : public QWidget
{
    Q_OBJECT
public:
    explicit EditWindow(std::weak_ptr<Settings>, QWidget *parent = 0);
    void loadNotes(const QDate &date, std::unique_ptr<QList<Note*>> list);
    void loadNote(Note * const note);

signals:
    void noteAdded(Note * const note);
    void noteEdited();

public slots:    
    void resizeMe();
    void addNote();

private slots:
    void loadFields(int index = -1);
    void showRepeatGroupBoxContent(const bool on);
    void showNotificationGroupBoxContent(const bool on);
    void refreshNotificationStartDate();

private:
    void setUI();
    void createRepeatGroupBox();
    void createNotificationGroupBox();
    void fillNoteSelector();    
    void resizeTimer();

    std::shared_ptr<Settings>       settings;
    Note                            *currentNote;
    QDate                           currentDate;
    std::unique_ptr<QList<Note*>>   noteList;

    // Inteface
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

};

#endif // EDITWINDOW_H
