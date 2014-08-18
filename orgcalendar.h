#ifndef ORGCALENDAR_H
#define ORGCALENDAR_H

#include <QCalendarWidget>
#include <QPainter>
#include "notebook.h"
#include "memory"

class orgCalendar : public QCalendarWidget
{
    Q_OBJECT
public:
    explicit orgCalendar(const std::weak_ptr<Notebook>, QWidget *parent = 0);

protected:
    void paintCell(QPainter * painter, const QRect & rect, const QDate & date) const;

private:
    std::shared_ptr<Notebook> notes;

signals:

public slots:
    void showToday();

};

#endif // ORGCALENDAR_H
