#ifndef SETTINGS_H
#define SETTINGS_H
#include "QString"

class Settings
{
public:
    enum repeatedDisplay{All, Future, Closest};
    bool autorun;
    QString dateFormat;
    repeatedDisplay rDisplay;
    void reset()
    {
        autorun = true;
        dateFormat = "dd/MM/yyyy";
        rDisplay = repeatedDisplay::Future;
    }
};

#endif // SETTINGS_H
