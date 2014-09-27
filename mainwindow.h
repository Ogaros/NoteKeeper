#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "calendar.h"
#include "editwindow.h"
#include "deletedialogue.h"
#include "settingswindow.h"
#include "notelistwindow.h"
#include "note.h"
#include "notebook.h"
#include "memory"
#include "settings.h"

enum class DeleteOption{One, Outdated, All};
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

signals:
    void noteDeleted();
    void noteEdited();

public slots:
    void openEditWindow(Note * const note = nullptr);
    void openSettingsWindow();
    void openAllNotesWindow();
    void openAboutWindow();
    void openDeleteDialogue();
    int  showDeleteMessageBox(const DeleteOption);
    void showNotes();
    void showClosestNote();
    void addNote(Note * const note);
    void saveNotes();
    void deleteNoteFromListWindow(Note * const note);
    void deleteAll();
    void deleteOutdated();
    int  showExitWOSavingMessageBox();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void switchButtons();    
    void resizeMe();
    void closeProgram();
    void showFromTray();
    void showTrayMessage();
    void deleteNote(Note * const note);
    void deleteNotes(std::shared_ptr<QList<Note*>> noteList);
    void iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void setUI();
    void moveToCenter(QWidget * const window);
    void createMenu();
    void createTrayIcon();
    void createStatusBar();
    void createButtonLayout();
    void createCalendar();
    void createScrollArea();
    void addNoteLabel();    
    void resizeTimer();

    bool isChanged;
    bool isClosing;
    std::shared_ptr<Notebook>       notes;
    std::shared_ptr<Settings>       settings;    

    //Interface
    QList<QLabel*>                  noteLabels;
    QHBoxLayout                     *mainLayout;
    QVBoxLayout                     *leftLayout;
    QVBoxLayout                     *buttonsLayout;
    Calendar                        *cal;
    QPushButton                     *addButton;
    QPushButton                     *editButton;
    QPushButton                     *removeButton;
    QPushButton                     *todayButton;
    QPushButton                     *quitButton;
    QLabel                          *noteTextTitle;
    QScrollArea                     *scrollArea;
    QVBoxLayout                     *scrollLayout;
    QLabel                          *statusLabel;
    QSystemTrayIcon                 *trayIcon;
    QMenu                           *trayIconMenu;
    QAction                         *quitAction;
    QAction                         *notificationsAction;
    QAction                         *openAction;
    QTimer                          *trayTimer;
};

#endif // MAINWINDOW_H
