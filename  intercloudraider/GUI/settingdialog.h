#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include "GUI/mylabel2.h"
#include "DBContr/databasecontrollersetting.h"

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();
    
private slots:
    void on_lineEdit_textEdited(const QString &arg1);

    void on_lineEdit_2_textEdited(const QString &arg1);

    void on_lineEdit_3_textEdited(const QString &arg1);

    void on_lineEdit_4_textEdited(const QString &arg1);

    void on_lineEdit_5_textEdited(const QString &arg1);

    void on_lineEdit_6_textEdited(const QString &arg1);

    void on_buttonBox_accepted();

private:
    Ui::SettingDialog *ui;

    QStringList key_secret;
};

#endif // SETTINGDIALOG_H
