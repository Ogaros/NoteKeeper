#include "calendar.h"

Calendar::Calendar(const std::weak_ptr<Notebook> notes, const std::weak_ptr<Settings> settings, QWidget *parent) :
    QCalendarWidget(parent)
{
    this->notes = notes.lock();    
    this->settings = settings.lock();
    setFirstDayOfWeek(Qt::Monday);
    setGridVisible(true);
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
}

void Calendar::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
{
    QCalendarWidget::paintCell(painter, rect, date);
    int penSize = 3;
    if(date == QDate::currentDate())
    {
        painter->save ();        
        QPen pen(Qt::blue, penSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter->setPen(pen);
        painter->setBrush(Qt::transparent);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawRect(QRect(rect.left() + penSize, rect.top() + penSize,
                                rect.width() - penSize*2, rect.height() - penSize*2));
        painter->restore ();
    }
    int count = notes->contains(date);
    if(count > 0)
    {
        painter->save ();
        QRect circleRect = rect;
        int cellHeight = (rect.top() - rect.bottom()) / 2;
        circleRect.setLeft(rect.center().x() - cellHeight);
        circleRect.setRight(rect.center().x() + cellHeight);
        circleRect.setBottom(circleRect.bottom() - 3);
        circleRect.setTop(circleRect.top() + 3);
        QPen pen(Qt::red, penSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter->setPen(pen);
        painter->setBrush(Qt::transparent);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->drawEllipse(circleRect);
        if(settings->showNumber)
        {            
            painter->drawText(QRect(rect.left(), rect.top(), rect.width() - 7, rect.height() - 5), Qt::AlignBottom | Qt::AlignRight, QString::number(count));
        }
        painter->restore ();
    }
}

void Calendar::showToday()
{
    setSelectedDate(QDate::currentDate());
    setFocus();
}
