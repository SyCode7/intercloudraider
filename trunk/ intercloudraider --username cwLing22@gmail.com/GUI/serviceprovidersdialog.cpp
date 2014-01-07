#include "serviceprovidersdialog.h"
#include "ui_serviceprovidersdialog.h"

ServiceProvidersDialog::ServiceProvidersDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServiceProvidersDialog)
{
    ui->setupUi(this);
    setWindowTitle("Select Cloud Storage Provider");
    this->setFixedSize(350, 150);
    ui->label->setText(QString("Please select 1 provider:"));
    ui->okButton->setEnabled(false);
}

ServiceProvidersDialog::~ServiceProvidersDialog()
{
    delete ui;
}

void ServiceProvidersDialog::on_dropboxradioButton_clicked()
{
    provider.append("Dropbox");

    ui->okButton->setEnabled(true);
}

void ServiceProvidersDialog::on_GoogleDriveradioButton_clicked()
{
    provider.append("Google Drive");

    ui->okButton->setEnabled(true);
}

void ServiceProvidersDialog::on_UbuntuOneradioButton_clicked()
{
    provider.append("SkyDrive");
    provider.append("Ubuntu");

    ui->okButton->setEnabled(true);
}

void ServiceProvidersDialog::on_okButton_clicked()
{
    this->close();
}
