#ifndef NOTETREEITEM_H
#define NOTETREEITEM_H

#include "memory"
#include "QTreeWidgetItem"
#include "settings.h"

class NoteTreeItem : public QTreeWidgetItem
{
public:
    NoteTreeItem(const std::weak_ptr<Settings> settings)
    {
        this->settings = settings.lock();
        frequencyMap.emplace("no", 0);
        frequencyMap.emplace("weekly", 1);
        frequencyMap.emplace("monthly", 2);
        frequencyMap.emplace("yearly", 3);
    }

private:
    std::shared_ptr<Settings> settings;
    std::map<QString, int> frequencyMap;
    bool operator <(const QTreeWidgetItem &other) const
    {
        int column = treeWidget()->sortColumn();
        QString colName = treeWidget()->headerItem()->text(column);
        QString thisText = text(column);
        QString otherText = other.text(column);
        if(colName == "Date")
        {
            if(settings == nullptr)
                throw std::bad_typeid("Sorting by date without settings pointer");
            else
                return QDate::fromString(thisText, settings->dateFormat) < QDate::fromString(otherText, settings->dateFormat);
        }
        else if(colName == "Repeated")
        {
            return frequencyMap.at(thisText) < frequencyMap.at(otherText);
        }
        else if(colName == "Notification")
        {
            if(thisText == "no" && otherText != "no")
                return true;
            else if(otherText == "no")
                return false;
            else
            {
                QString aaa = thisText.section(' ',0,0);
                QString bbb = otherText.section(' ',0,0);
                return aaa.toInt() < bbb.toInt();
            }
        }
        else
        {
            return thisText < otherText;
        }
    }
};

#endif // NOTETREEITEM_H
