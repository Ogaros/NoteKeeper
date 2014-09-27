QT += widgets

HEADERS += \
    mainwindow.h \
    main.h \
    calendar.h \
    widgetList.h \
    note.h \
    notebook.h \
    editwindow.h \
    deletedialogue.h \
    settingswindow.h \
    settings.h \
    notelistwindow.h \
    notetreeitem.h

SOURCES += \
    mainwindow.cpp \
    main.cpp \
    calendar.cpp \
    note.cpp \
    notebook.cpp \
    editwindow.cpp \
    deletedialogue.cpp \
    settingswindow.cpp \
    settings.cpp \
    notelistwindow.cpp

RESOURCES += \
    resources.qrc

FORMS += \
    deletedialogue.ui \
    SettingsWindow.ui \
    notelistwindow.ui

CONFIG += C++11

win32:RC_ICONS += images/Calendar.ico
