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

bool Notebook::addNote(Note *n)
{
    if(n == nullptr)
        return false;
    notes.append(n);
    try
    {
        sort();
    }
    catch(...)
    {
        throw;
    }
    return true;
}
bool Notebook::addNote(const QDate date, const QString text, const nFrequency frequency,
                       const bool notifEnabled, const int daysPrior)
{
    Note *n = new Note(date, text, frequency, notifEnabled, daysPrior);
    return addNote(n);
}

void Notebook::loadNotes()
{
    QFile file("OrgNotes.xml");
    if(!file.exists())
        return;
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::exception("Failed to open OrgNotes.xml");
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
        throw std::exception(xml.errorString().toLocal8Bit());
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
        throw std::exception("Failed to open OrgNotes.xml");
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

QString* Notebook::getTextFromDate(const QDate &date) const
{
    for(auto note : notes)
    {
        if(note->date == date)
            return &(note->text);
        if(note->date > date)
            break;
    }
    return nullptr;
}

std::unique_ptr<QList<Note*>> Notebook::getNotesFromDate(const QDate &date) const
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

bool Notebook::deleteNote(Note *note)
{
   delete note;
   return notes.removeOne(note);
}

int Notebook::deleteAll()
{
    int deleted = notes.size();
    for(auto iNote = notes.begin(); iNote != notes.end(); iNote++)
    {
        delete *iNote;
        notes.erase(iNote);
    }
    return deleted;
}

int Notebook::deleteOutdated(const QDate &date)
{
    int deleted = 0;
    for(auto iNote = notes.begin(); iNote != notes.end(); iNote++)
    {
        if((*iNote)->date < date)
        {
            delete *iNote;
            notes.erase(iNote);
            ++deleted;
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

