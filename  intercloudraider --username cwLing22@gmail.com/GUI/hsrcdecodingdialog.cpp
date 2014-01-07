#include "hsrcdecodingdialog.h"
#include "ui_hsrcdecodingdialog.h"

HSRCDecodingDialog::HSRCDecodingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HSRCDecodingDialog)
{
    ui->setupUi(this);

    setWindowTitle("File Retrieve");

    displayTable();

    file_id = 0; // file_id counted from 1
}

HSRCDecodingDialog::~HSRCDecodingDialog()
{
    delete ui;
}

void HSRCDecodingDialog::displayTable()
{    
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("metadata.db");

        if(db.open())
        {
            model = new QSqlQueryModel();
            model->setQuery("SELECT * FROM metadata");

            ui->treeView->setModel(model);

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void HSRCDecodingDialog::on_treeView_clicked(const QModelIndex &index)
{
    file_id = model->record(index.row()).value(0).toInt();
}

void HSRCDecodingDialog::on_outputbrowse_clicked()
{
    folder = ui->outputedit->text();

    folderbrowserDialog fdialog;
    connect(&fdialog, SIGNAL(selectedPath(QString)), ui->outputedit, SLOT(setText(QString)));
    fdialog.exec();

    if(folder.isEmpty() == true && QDir(fdialog.getPath()).exists() == true)
    {
        folder = fdialog.getPath();
    }
    else
    {
        if(QDir(folder).exists() == true && QDir(fdialog.getPath()).exists() == true)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Changing Selected Path",
                                          "Are you sure you want to change the selected folder?",
                                  QMessageBox::Ok|QMessageBox::Cancel);
            if(reply == QMessageBox::Ok)
            {
                ui->outputedit->clear();
                ui->outputedit->setText(fdialog.getPath());
                folder = fdialog.getPath();
            }
            else
            {
                ui->outputedit->clear();
                ui->outputedit->setText(folder);
            }
        }
    }
}

void HSRCDecodingDialog::on_buttonBox_accepted()
{
    if(file_id <= 0)
    {
        QMessageBox::warning(this, "Warning",
                             "Please select an entry from the operation log!");
    }
    else
    {
        folder = ui->outputedit->text();

        if(QDir(folder).exists() == false || folder.isEmpty() == true)
        {
            QMessageBox::warning(this, "Warning",
                                 "Please select a correct folder!");
        }
        else
        {
            success = true;

            this->close();
        }
    }
}

void HSRCDecodingDialog::on_buttonBox_rejected()
{
    this->close();
}
