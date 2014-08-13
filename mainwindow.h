#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "widgetList.h"
#include "orgcalendar.h"
#include "addnotewindow.h"
#include "note.h"
#include "notebook.h"

enum DeleteOption
{
    One,
    Outdated,
    All
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

signals:
    void noteDeleted();

public slots:
    void showAddWindow();
    void addNote(Note *n, const bool);
    void showNote();
    void showClosestNote();
    void saveNotes();
    void deleteNode();
    void deleteAll();
    void deleteOutdated();
    int showDeleteMessageBox(const DeleteOption);
    int showExitWOSavingMessageBox();

private slots:
    void switchButtons();
    void resizeMe();    

protected:
    void closeEvent(QCloseEvent *event);


private:
    void setUI();
    void moveToCenter(QWidget *);
    void createMenu();
    void createStatusBar();
    bool isChanged;

    Notebook      *notes;
    addNoteWindow *addWindow = nullptr;
    QVBoxLayout   *mainLayout;
    QHBoxLayout   *topLayout;
    QVBoxLayout   *buttonsLayout;
    orgCalendar   *cal;
    QPushButton   *addButton;
    QPushButton   *editButton;
    QPushButton   *removeButton;
    QPushButton   *todayButton;
    QPushButton   *quitButton;
    QLabel        *noteText;
    QLabel        *statusLabel;   
};

#endif // MAINWINDOW_H
