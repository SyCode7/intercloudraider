#include "operationsdialog.h"
#include "ui_operationsdialog.h"

OperationsDialog::OperationsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OperationsDialog)
{
    ui->setupUi(this);
    setWindowTitle("Start New Operation");
    this->setFixedSize(400, 320);

    ui->widget1->setStyleSheet("background-color: rgb(190, 190, 190)");

    ui->BackupradioButton->setChecked(true);
    operationMode = 1;
    ui->label1->setText("Back up selected files/folder");
    ui->label2->setText("to cloud storage providers.");

    MyLabel2 *Infolabel = new MyLabel2(2, "More information...", this);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(Infolabel);

    ui->Infoframe->setLayout(hlayout);

    QFont font;
    font.setUnderline(true);
    Infolabel->setFont(font);
}

OperationsDialog::~OperationsDialog()
{
    delete ui;
}


void OperationsDialog::on_BackupradioButton_clicked()
{
    operationMode = 1;

    ui->label1->setText("Backup selected files/folder");
    ui->label2->setText("to cloud storage providers.");
}

void OperationsDialog::on_RetrieveradioButton_clicked()
{
    operationMode = 2;

    ui->label1->setText("Retrieve files/folder from");
    ui->label2->setText("cloud storage providers.");
}

void OperationsDialog::on_okbutton_clicked()
{
    if(operationMode == 1)
    {
        HSRCEncodingDialog edialog;
        edialog.exec();

        success = edialog.success;
        file_idList = edialog.file_idList;
    }
    else if(operationMode == 2)
    {
        HSRCDecodingDialog ddialog;
        ddialog.exec();

        success = ddialog.success;
        file_id = ddialog.file_id;
        folder = ddialog.folder;
    }

    this->close();
}
