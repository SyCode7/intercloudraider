#include "filebrowserdialog.h"
#include "ui_filebrowserdialog.h"

FileBrowserDialog::FileBrowserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileBrowserDialog)
{
    ui->setupUi(this);

    setWindowTitle("Browse File/Folder");

    QString sPath = "F:/";
    dirmodel = new QFileSystemModel(this);
    dirmodel->setRootPath(sPath);
    ui->treeView->setModel(dirmodel);

    ui->okbutton->setEnabled(false);
    ui->makedirbutton->setEnabled(false);
}

FileBrowserDialog::~FileBrowserDialog()
{
    delete ui;
}

void FileBrowserDialog::on_treeView_clicked(const QModelIndex &index)
{
    mpath = dirmodel->fileInfo(index).absoluteFilePath();
    if(QFile(mpath).exists())
        ui->okbutton->setEnabled(true);
    else
        ui->okbutton->setEnabled(false);

    if(QDir(mpath).exists())
        ui->makedirbutton->setEnabled(true);
    else
        ui->makedirbutton->setEnabled(false);
}

void FileBrowserDialog::on_makedirbutton_clicked()
{
    QDir mdir;
    QString dir = "/New Folder";
    dir = mpath.append(dir);
    if(!QDir(dir).exists())
    {
        mdir.mkpath(dir);
    }
}

void FileBrowserDialog::on_okbutton_clicked()
{
    path = mpath;
    emit (selectedPath(path));
}

