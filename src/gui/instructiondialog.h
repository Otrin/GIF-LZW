#ifndef INSTRUCTIONDIALOG_H
#define INSTRUCTIONDIALOG_H

#include <QDialog>

namespace Ui {
class InstructionDialog;
}

class InstructionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InstructionDialog(QWidget *parent = 0);
    ~InstructionDialog();

private:
    Ui::InstructionDialog *ui;
};

#endif // INSTRUCTIONDIALOG_H
