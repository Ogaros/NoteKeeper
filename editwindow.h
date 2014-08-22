#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include "widgetList.h"
#include "note.h"
#include "QRegExpValidator"

class EditWindow : public QWidget
{
    Q_OBJECT
public:
    explicit EditWindow(QWidget *parent = 0);
    void loadFields(const QDate&, Note*);
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
    void notificationDaysTextChange(const QString);
    void showRepeatGroupBoxContent(const bool on);
    void showNotificationGroupBoxContent(const bool on);

private:
    void setUI();
    void createRepeatGroupBox();
    void createNotificationGroupBox();

    Note                *currentNote;
    QLabel              *dateLabel;
    QDateEdit           *selectedDate;
    QLabel              *noteTextLabel;
    QLabel              *errorLabel;
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
    QVBoxLayout         *mainLayout;
    QDialogButtonBox    *noteAddButtons;
};

#endif // EDITWINDOW_H
