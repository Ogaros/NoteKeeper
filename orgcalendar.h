#ifndef ORGCALENDAR_H
#define ORGCALENDAR_H

#include <QCalendarWidget>
#include <QPainter>
#include "notebook.h"

class orgCalendar : public QCalendarWidget
{
    Q_OBJECT
public:
    explicit orgCalendar(Notebook*, QWidget *parent = 0);

protected:
    void paintCell(QPainter * painter, const QRect & rect, const QDate & date) const;

private:
    Notebook *notes;

signals:

public slots:
    void showToday();

};

#endif // ORGCALENDAR_H
