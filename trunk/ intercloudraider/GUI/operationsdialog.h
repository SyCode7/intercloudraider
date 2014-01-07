#ifndef OPERATIONSDIALOG_H
#define OPERATIONSDIALOG_H

#include <QDialog>
#include <QFont>

#include "GUI/mylabel2.h"
#include "GUI/hsrcdecodingdialog.h"
#include "GUI/hsrcencodingdialog.h"

namespace Ui {
class OperationsDialog;
}

class OperationsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit OperationsDialog(QWidget *parent = 0);
    ~OperationsDialog();

    int operationMode;
    bool success;

    // encoding:
    QList< int> file_idList;

    // decoding:
    int file_id;
    QString folder;

private slots:
    void on_BackupradioButton_clicked();

    void on_RetrieveradioButton_clicked();

    void on_okbutton_clicked();

private:
    Ui::OperationsDialog *ui;
};

#endif // OPERATIONSDIALOG_H
