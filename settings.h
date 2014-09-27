#ifndef SETTINGS_H
#define SETTINGS_H

#include "QString"
#include "QSettings"
#include "QApplication"
#include "QFile"

class Settings
{
public:
    enum repeatedDisplay{All = 0, Future = 1, Closest = 2};

    Settings();
    void restore();
    void load();
    void save() const;

    bool autorun;
    QString dateFormat;
    repeatedDisplay rDisplay;
    bool showNumber;

private:
    const QString filePath = QApplication::applicationDirPath() + "/settings.ini";
};

#endif // SETTINGS_H
