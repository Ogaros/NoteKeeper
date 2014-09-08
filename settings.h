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
    bool autorun;
    QString dateFormat;
    repeatedDisplay rDisplay;
    void restore();
    void load();
    void save();
private:
    const QString filePath = QApplication::applicationDirPath() + "/settings.ini";
};

#endif // SETTINGS_H
