#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include "widgetList.h"
#include "note.h"

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
    void hideLayoutItems(const bool on);

private:
    void setUI();
    void createNotificationGroupBox();

    Note *currentNote;
    QLabel *dateLabel;
    QDateEdit *selectedDate;
    QLabel *noteTextLabel;
    QLabel *errorLabel;
    QTextEdit *noteText;
    QButtonGroup *notifyRButtonGroup;
    QGroupBox *notifyGroupBox;
    QRadioButton *notifyRBOnce;
    QLabel *notifyOnceDateLabel;
    QDateTimeEdit *notifyOnceDate;
    QRadioButton *notifyRBRepeat;
    QDateEdit *notifyRepeatedDate;
    QLabel *notifyRepeatedDateLabel;
    QLabel *notifyRepeatedTimeLabel;
    QTimeEdit *notifyRepeatedTime;
    QVBoxLayout *mainLayout;
    QVBoxLayout *notifyLayout;
    QDialogButtonBox *noteAddButtons;
};

#endif // EDITWINDOW_H
