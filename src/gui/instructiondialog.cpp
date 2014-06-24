#include "instructiondialog.h"
#include "ui_instructiondialog.h"

InstructionDialog::InstructionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstructionDialog)
{
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);
}

InstructionDialog::~InstructionDialog()
{
    delete ui;
}
