#ifndef ADDNOTEWINDOW_H
#define ADDNOTEWINDOW_H

#include "widgetList.h"
#include "note.h"

class addNoteWindow : public QWidget
{
    Q_OBJECT
public:
    explicit addNoteWindow(QWidget *parent = 0);
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
    Note *currentNote;
    void setUI();
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

#endif // ADDNOTEWINDOW_H
