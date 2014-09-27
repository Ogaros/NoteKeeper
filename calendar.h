#ifndef ORGCALENDAR_H
#define ORGCALENDAR_H

#include "QCalendarWidget"
#include "QPainter"
#include "notebook.h"
#include "memory"
#include "settings.h"

class Calendar : public QCalendarWidget
{
    Q_OBJECT
public:
    explicit Calendar(const std::weak_ptr<Notebook>, const std::weak_ptr<Settings>, QWidget *parent = 0);

public slots:
    void showToday();

protected:
    void paintCell(QPainter *painter, const QRect &rect, const QDate &date) const;

private:
    std::shared_ptr<Notebook> notes;
    std::shared_ptr<Settings> settings;
};

#endif // ORGCALENDAR_H
