#include "deletedialogue.h"
#include "ui_deletedialogue.h"

DeleteDialogue::DeleteDialogue(QStringList list, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DeleteDialogue)
{
    ui->setupUi(this);
    ui->comboBox->addItems(list);
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(setIndex()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(setIndex()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SIGNAL(ready()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SIGNAL(ready()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

DeleteDialogue::~DeleteDialogue()
{
    delete ui;
}

int DeleteDialogue::getIndex()
{
    return index;
}

void DeleteDialogue::setIndex()
{
    QObject::sender() == ui->deleteButton ? index = ui->comboBox->currentIndex() : index = -1;
}
