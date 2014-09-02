#include "notebook.h"

Notebook::Notebook()
{
    try
    {
        loadNotes();
    }
    catch(...)
    {
        throw;
    }
}

Notebook::~Notebook()
{
    deleteAll();
}

void Notebook::addNote(Note *n)
{
    notes.append(n);
    try
    {
        sort();
    }
    catch(...)
    {
        throw;
    }
}
void Notebook::addNote(const QDate date, const QString text, const nFrequency frequency,
                       const bool notifEnabled, const int daysPrior)
{
    Note *n = new Note(date, text, frequency, notifEnabled, daysPrior);
    addNote(n);
}

void Notebook::loadNotes()
{
    QFile file("OrgNotes.xml");
    if(!file.exists())
        return;
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::runtime_error("Failed to open OrgNotes.xml");
    QXmlStreamReader xml(&file);

    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();
        if(token == QXmlStreamReader::StartDocument)
            continue;
        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "notebook")
                continue;
            if(xml.name() == "note")
            {
                notes.append(parseNote(xml));
            }
        }
    }
    if(xml.hasError())
        throw std::runtime_error(xml.errorString().toStdString());
    file.close();
}

Note* Notebook::parseNote(QXmlStreamReader &xml) const
{
    Note *n = nullptr;
    QDate date;
    QString text;
    nFrequency frequency;
    bool notifEnabled;
    int daysPrior;

    QXmlStreamAttributes attributes = xml.attributes();
    if(attributes.hasAttribute("date"))
        date = QDate::fromString(attributes.value("date").toString(),"dd/MM/yyyy");
    xml.readNext();
    while(xml.tokenType() != QXmlStreamReader::EndElement || xml.name() != "note")
    {
        if(xml.tokenType() == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "text")
            {
                xml.readNext();
                text = xml.text().toString();
            }
            else if(xml.name() == "frequency")
            {
                xml.readNext();
                frequency = static_cast<nFrequency>(xml.text().toInt());
            }
            else if(xml.name() == "notifEnabled")
            {
                xml.readNext();
                notifEnabled = xml.text().toInt();
            }
            else if(xml.name() == "daysPrior")
            {
                xml.readNext();
                daysPrior = xml.text().toInt();
            }
        }
        xml.readNext();
    }
    n = new Note(date, text, frequency, notifEnabled, daysPrior);
    return n;
}

void Notebook::saveNotes() const
{
    QFile file("OrgNotes.xml");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        throw std::runtime_error("Failed to open OrgNotes.xml");
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);

    xml.writeStartDocument();
    xml.writeStartElement("notebook");

    for (Note *n : notes)
    {
        xml.writeStartElement("note");
        xml.writeAttribute("date",n->date.toString("dd/MM/yyyy"));
        xml.writeTextElement("text",n->text);
        xml.writeTextElement("frequency",QString::number(static_cast<int>(n->frequency)));
        xml.writeTextElement("notifEnabled",QString::number(n->notifEnabled));
        xml.writeTextElement("daysPrior",QString::number(n->daysPrior));
        xml.writeEndElement();
    }

    xml.writeEndElement();
    xml.writeEndDocument();
    file.flush();
    file.close();
}

void Notebook::sort()
{
    try
    {
        std::stable_sort(notes.begin(), notes.end(), ptrLess());
    }
    catch(...)
    {
        throw;
    }
}

std::unique_ptr<QList<Note*>> Notebook::getNotesFromDate(const QDate & date) const
{
    std::unique_ptr<QList<Note*>> list(new QList<Note*>);
    for(auto note : notes)
    {
        if(noteOnDate(note, date))
        {
            list->append(note);
        }
    }
    return list;
}

std::unique_ptr<QList<Note*>> Notebook::getNotificationsFromDate(const QDate & date) const
{
    std::unique_ptr<QList<Note*>> list(new QList<Note*>);
    for(auto note : notes)
    {
        if(notificationOnDate(note, date))
        {
            list->append(note);
        }
    }
    return list;
}

bool Notebook::contains(const QDate &date) const
{
    for (auto note : notes)
    {
        if(noteOnDate(note, date))
            return true;
    }
    return false;
}

bool Notebook::noteOnDate(Note * const note, const QDate & date) const
{
    if((note->date == date) ||
       (note->frequency == nFrequency::Week && note->date.daysTo(date) % 7 == 0) ||
       (note->frequency == nFrequency::Month && date.day() == note->date.day()) ||
       (note->frequency == nFrequency::Year && date.day() == note->date.day()) && date.month() == note->date.month())
        return true;
    return false;
}

bool Notebook::notificationOnDate(Note * const note, const QDate & date) const
{
    if(note->notifEnabled)
    {
        if(note->date == date)
            return true;
        auto showNotification = [&](QDate &noteDate)->bool{return noteDate >= date && date >= noteDate.addDays(-note->daysPrior);};
        if(note->frequency == nFrequency::Once)
        {
            return showNotification(note->date);
        }
        else
        {
            QDate (*next)(QDate&, int&);
            if(note->frequency == nFrequency::Week)
                next = [](QDate &d, int &sign){return d.addDays(sign * 7);};
            else if(note->frequency == nFrequency::Month)
                next = [](QDate &d, int &sign){return d.addMonths(sign * 1);};
            else
                next = [](QDate &d, int &sign){return d.addYears(sign * 1);};
            int sign = note->date > date ? -1 : 1;
            QDate closestDate = note->date;
            QDate tempDate = next(closestDate, sign);
            if(note->date > date)
            {
                while(tempDate >= date)
                {
                    closestDate = tempDate;
                    tempDate = next(tempDate, sign);
                }
            }
            else
            {
                while(tempDate < date)
                {
                    tempDate = next(tempDate, sign);
                }
                closestDate = tempDate;
            }
            note->date = closestDate;
            return showNotification(closestDate);
        }
    }
    return false;
}

bool Notebook::deleteNote(Note *note)
{
    if(notes.contains(note))
    {
        delete note;
        return notes.removeOne(note);
    }
    return false;
}

int Notebook::deleteAll()
{
    int deleted = notes.size();
    for(auto note : notes)
    {
        delete note;
    }
    notes.clear();
    return deleted;
}

int Notebook::deleteOutdated(const QDate &date)
{
    int deleted = 0;
    for(auto note : notes)
    {
        if(note->date < date)
        {
            if(note->frequency == nFrequency::Once)
            {
                delete note;
                notes.removeOne(note);
                ++deleted;
            }
        }
        else
            return deleted;
    }
    return deleted;
}

Note* Notebook::findClosest(const QDate &date) const
{
    for(auto note : notes)
    {
        if(note->date > date)
            return note;
    }
    return nullptr;
}

