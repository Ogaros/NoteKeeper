#ifndef DELETEDIALOGUE_H
#define DELETEDIALOGUE_H

#include <QWidget>

namespace Ui {
class Deletedialogue;
}

class DeleteDialogue : public QWidget
{
    Q_OBJECT

public:
    explicit DeleteDialogue(QStringList, QWidget *parent = 0);
    ~DeleteDialogue();
    int getIndex();

private slots:
    void setIndex();

signals:
    void ready();

private:
    Ui::Deletedialogue *ui;    
    int index;
};

#endif // DELETEDIALOGUE_H
