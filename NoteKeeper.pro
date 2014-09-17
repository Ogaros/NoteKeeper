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
    settings.h

SOURCES += \
    mainwindow.cpp \
    main.cpp \
    calendar.cpp \
    note.cpp \
    notebook.cpp \
    editwindow.cpp \
    deletedialogue.cpp \
    settingswindow.cpp \
    settings.cpp

RESOURCES += \
    resources.qrc

FORMS += \
    deletedialogue.ui \
    SettingsWindow.ui

CONFIG += C++11
