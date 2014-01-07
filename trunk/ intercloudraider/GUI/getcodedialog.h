#ifndef GETCODEDIALOG_H
#define GETCODEDIALOG_H

#include <QDialog>

namespace Ui {
class GetCodeDialog;
}

class GetCodeDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit GetCodeDialog(QWidget *parent = 0);
    ~GetCodeDialog();

    QString code;
    
private slots:
    void on_buttonBox_accepted();

private:
    Ui::GetCodeDialog *ui;
};

#endif // GETCODEDIALOG_H
