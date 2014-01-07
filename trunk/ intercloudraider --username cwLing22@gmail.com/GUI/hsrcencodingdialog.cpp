#include "hsrcencodingdialog.h"
#include "ui_hsrcencodingdialog.h"

HSRCEncodingDialog::HSRCEncodingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HSRCEncodingDialog)
{
    ui->setupUi(this);

    setWindowTitle("File Backup");
}

HSRCEncodingDialog::~HSRCEncodingDialog()
{
    delete ui;
}

void HSRCEncodingDialog::on_inputbrowse_clicked()
{
    inputpath = ui->inputedit->text();

    FileBrowserDialog fdialog;
    connect(&fdialog, SIGNAL(selectedPath(QString)), ui->inputedit, SLOT(setText(QString)));
    fdialog.exec();

    if(inputpath.isEmpty() == true && QFile(fdialog.getPath()).exists() == true)
    {
        inputpath = fdialog.getPath();
    }
    else
    {
        if(QFile(inputpath).exists() == true && QFile(fdialog.getPath()).exists() == true)
        {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Changing Selected Path",
                                          "Are you sure you want to change the selected path?",
                                  QMessageBox::Ok|QMessageBox::Cancel);
            if(reply == QMessageBox::Ok)
            {
                ui->inputedit->clear();
                ui->inputedit->setText(fdialog.getPath());
                inputpath = fdialog.getPath();
            }
            else
            {
                ui->inputedit->clear();
                ui->inputedit->setText(inputpath);
            }
        }
    }
}

int HSRCEncodingDialog::pathIsDir(QString path)
{
    QDir dir(path);
    QFile file(path);

    if(dir.exists())
    {
        dir.setFilter(QDir::Files|QDir::NoDot|QDir::NoDotDot);
        dir.setSorting(QDir::Name|QDir::Size);

        filelist = dir.entryInfoList();

        // if the path is a directory:
        return 1;
    }

    else
    {
        if(file.exists())
        {
            // if the path is a file:
            return 0;
        }
        else
        {
            return -1;
        }
    }
}

void HSRCEncodingDialog::on_buttonBox_accepted()
{
    inputpath = ui->inputedit->text();

    if(!QFile(inputpath).exists())
    {
        QMessageBox::warning(this, "Warning",
                             "Please select a correct input file/folder!");

        FileBrowserDialog fdialog;
        connect(&fdialog, SIGNAL(selectedPath(QString)),
                ui->inputedit, SLOT(setText(QString)));
        fdialog.exec();

        inputpath = fdialog.getPath();
    }
    else
    {
        if(pathIsDir(inputpath) == 1)
        {           
            if(filelist.size() == 0)
            {
                QMessageBox::warning(this, "Warning",
                                     "This is an empty folder!");

            }
            else
            {
                for(int i = 0; i < filelist.size(); i++)
                {
                    QFileInfo fileInfo(filelist.at(i));

                    // create an operation log to store the encoding
                    // information per operation by using SQLite
                    QList<QVariant> result;

                    DatabaseControllerMetadata dbcontr_m;
                    result = dbcontr_m.insertEntries(fileInfo.fileName(), fileInfo.size(),
                                                     fileInfo.suffix(), fileInfo.absoluteFilePath());

                    if(result.first().toBool() == true)
                    {
                        success = result.first().toBool();
                        file_idList.append(result.last().toInt());
                    }
                    else
                    {
                        success = result.first().toBool();
                    }
                }
            }
        }
        else if(pathIsDir(inputpath) == 0)
        {
            QFileInfo fileInfo(inputpath);

            // create an operation log to store the encoding
            // information per operation by using SQLite
            QList<QVariant> result;

            DatabaseControllerMetadata dbcontr_m;
            result = dbcontr_m.insertEntries(fileInfo.fileName(), fileInfo.size(),
                                             fileInfo.suffix(), fileInfo.absoluteFilePath());


            if(result.first().toBool() == true)
            {
                success = result.first().toBool();
                file_idList.append(result.last().toInt());
            }
            else
            {
                success = result.first().toBool();
            }
        }

        else
        {
            success = false;
        }

        this->close();
    }

}

void HSRCEncodingDialog::on_buttonBox_rejected()
{
    this->close();
}
