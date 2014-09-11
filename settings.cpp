#include "settings.h"

Settings::Settings()
{
    restore();
}

void Settings::restore()
{
    autorun = true;
    dateFormat = "dd/MM/yyyy";
    rDisplay = repeatedDisplay::Future;
    showNumber = false;
}

void Settings::save()
{
    QSettings settings(filePath, QSettings::IniFormat);
    settings.setValue("Autorun", autorun);
    settings.setValue("Date_format", dateFormat);
    settings.setValue("Display_repeated", static_cast<int>(rDisplay));
    settings.setValue("showNumber", showNumber);
}

void Settings::load()
{
    QFile file(filePath);
    if(file.exists())
    {
        QSettings settings(filePath, QSettings::IniFormat);
        autorun = settings.value("Autorun").toBool();
        dateFormat = settings.value("Date_format").toString();
        rDisplay = static_cast<repeatedDisplay>(settings.value("Display_repeated").toInt());
        showNumber = settings.value("showNumber").toBool();
    }
    else
    {
        save();
    }
}
