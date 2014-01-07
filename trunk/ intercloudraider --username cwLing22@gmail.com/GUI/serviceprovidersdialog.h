#ifndef SERVICEPROVIDERSDIALOG_H
#define SERVICEPROVIDERSDIALOG_H

#include <QDialog>
#include <QtGui>

namespace Ui {
class ServiceProvidersDialog;
}

class ServiceProvidersDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ServiceProvidersDialog(QWidget *parent = 0);

    ~ServiceProvidersDialog();

    QStringList provider;
    
private slots:
    void on_dropboxradioButton_clicked();

    void on_GoogleDriveradioButton_clicked();

    void on_UbuntuOneradioButton_clicked();

    void on_okButton_clicked();

private:
    Ui::ServiceProvidersDialog *ui;
};

#endif // SERVICEPROVIDERSDIALOG_H
