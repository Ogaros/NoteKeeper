QT += widgets

HEADERS += \
    mainwindow.h \
    main.h \
    orgcalendar.h \
    widgetList.h \
    note.h \
    notebook.h \
    editwindow.h \
    deletedialogue.h \
    settingswindow.h

SOURCES += \
    mainwindow.cpp \
    main.cpp \
    orgcalendar.cpp \
    note.cpp \
    notebook.cpp \
    editwindow.cpp \
    deletedialogue.cpp \
    settingswindow.cpp

RESOURCES += \
    resources.qrc

FORMS += \
    deletedialogue.ui \
    SettingsWindow.ui

CONFIG += C++11
