#include "orgcalendar.h"

orgCalendar::orgCalendar(Notebook *notes,QWidget *parent) :
    QCalendarWidget(parent)
{
    this->notes = notes;
    this->setFirstDayOfWeek(Qt::Monday);
    this->setGridVisible(true);
    this->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
}

void orgCalendar::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
    QCalendarWidget::paintCell(painter, rect, date);
    if(date == QDate::currentDate())
    {
        painter->save ();

        int penSize = 3;
        QPen pen(Qt::blue, penSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter->setPen(pen);
        painter->setBrush(Qt::transparent);
        painter->setRenderHint(QPainter::Antialiasing);


        painter->drawRect(QRect(rect.left() + penSize, rect.top() + penSize,
                                rect.width() - penSize*2, rect.height() - penSize*2));



        painter->restore ();
    }
    if(notes->contains(date))
    {
        painter->save ();

        QRect circleRect = rect;
        int cellHeight = (rect.top() - rect.bottom()) / 2;
        circleRect.setLeft(rect.center().x() - cellHeight);
        circleRect.setRight(rect.center().x() + cellHeight);
        circleRect.setBottom(circleRect.bottom() - 3);
        circleRect.setTop(circleRect.top() + 3);

        QPen pen(Qt::red, 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter->setPen(pen);
        painter->setBrush(Qt::transparent);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawEllipse(circleRect);



        painter->restore ();
    }
}

void orgCalendar::showToday()
{
    setSelectedDate(QDate::currentDate());
    setFocus();
}
