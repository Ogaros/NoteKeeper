#include "notebook.h"

Notebook::Notebook(QWidget *parent) : parent(parent)
{
    loadNotes();
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
    sort();


    return true;
}
bool Notebook::addNote(const QDate nDate, const QString nText)
{
    Note *n = new Note(nDate, nText);
    return addNote(n);
}
bool Notebook::addNote(const QDate nDate, const QString nText, const QDate notifDate,
                       const QTime notifTime, const  bool isRepeated)
{
    Note *n = new Note(nDate, nText, notifDate, notifTime, isRepeated);
    return addNote(n);
}

void Notebook::loadNotes()
{
    QFile file("OrgNotes.xml");
    if(!file.exists())
        return;
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        QMessageBox::critical(this->parent, "Notebook::loadNotes", "Failed to open OrgNotes.xml");
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
        QMessageBox::critical(this->parent, "Notebook::loadNotes", xml.errorString());
    file.close();
}

Note* Notebook::parseNote(QXmlStreamReader &xml) const
{
    Note *n = nullptr;
    QDate nDate;
    QString nText;
    QDate notifDate;
    QTime notifTime;
    bool isNotified;
    bool isRepeated;
    QXmlStreamAttributes attributes = xml.attributes();
    if(attributes.hasAttribute("date"))
        nDate = QDate::fromString(attributes.value("date").toString(),"dd/MM/yyyy");
    xml.readNext();
    while(xml.tokenType() != QXmlStreamReader::EndElement || xml.name() != "note")
    {
        if(xml.tokenType() == QXmlStreamReader::StartElement)
        {
            if(xml.name() == "text")
            {
                xml.readNext();
                nText = xml.text().toString();
            }
            else if(xml.name() == "notify")
            {
                xml.readNext();
                isNotified = xml.text().toInt();
                if(!isNotified)
                {
                    n = new Note(nDate, nText);
                    return n;
                }
            }
            else if(xml.name() == "repeated")
            {
                xml.readNext();
                isRepeated = xml.text().toInt();
            }
            else if(xml.name() == "notifDate")
            {
                xml.readNext();
                notifDate = QDate::fromString(xml.text().toString(),"dd/MM/yyyy");
            }
            else if(xml.name() == "notifTime")
            {
                xml.readNext();
                notifTime = QTime::fromString(xml.text().toString(),"hh:mm AP");
            }
        }
        xml.readNext();
    }

    n = new Note(nDate, nText, notifDate, notifTime, isRepeated);
    return n;

}

void Notebook::saveNotes() const
{
    QFile file("OrgNotes.xml");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        QMessageBox::critical(this->parent, "Notebook::saveNotes", "Failed to open OrgNotes.xml");
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);

    xml.writeStartDocument();
    xml.writeStartElement("notebook");

    for (Note *n : notes)
    {
        xml.writeStartElement("note");
        xml.writeAttribute("date",n->nDate.toString("dd/MM/yyyy"));
        xml.writeTextElement("text",n->nText);
        xml.writeTextElement("notify",QString::number(n->isNotified));
        xml.writeTextElement("repeated",QString::number(n->isRepeated));
        xml.writeTextElement("notifDate",n->notifDate.toString("dd/MM/yyyy"));
        xml.writeTextElement("notifTime",n->notifTime.toString("hh:mm AP"));
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
        qSort(notes.begin(), notes.end(), ptrLess());
    }
    catch(std::exception e)
    {
        QMessageBox::critical(parent, "Notebook::sort", QString::fromLocal8Bit(e.what()));
        qApp->quit();
    }
}

QString* Notebook::getTextFromDate(const QDate &date) const
{
    for(auto note : notes)
    {
        if(note->nDate == date)
            return &(note->nText);
        if(note->nDate > date)
            break;
    }
    return nullptr;
}

Note* Notebook::getNoteFromDate(const QDate &date) const
{
    for(auto note : notes)
    {
        if(note->nDate == date)
            return note;
        if(note->nDate > date)
            break;
    }
    return nullptr;
}

bool Notebook::contains(const QDate &date) const
{
    for (auto note : notes)
        if(note->nDate == date)
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
        if((*iNote)->nDate < date)
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
        if(note->nDate > date)
            return note;
    }
    return nullptr;
}

