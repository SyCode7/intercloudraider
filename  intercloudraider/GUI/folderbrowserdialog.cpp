#include "folderbrowserdialog.h"
#include "ui_folderbrowserdialog.h"

folderbrowserDialog::folderbrowserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::folderbrowserDialog)
{
    ui->setupUi(this);

    setWindowTitle("Select Folder");

    QString sPath = "F:/";
    dirmodel = new QFileSystemModel(this);
    dirmodel->setRootPath(sPath);
    dirmodel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
    ui->treeView->setModel(dirmodel);

    ui->okbutton->setEnabled(false);
    ui->makedirbutton->setEnabled(false);
}

folderbrowserDialog::~folderbrowserDialog()
{
    delete ui;
}

void folderbrowserDialog::on_treeView_clicked(const QModelIndex &index)
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

void folderbrowserDialog::on_makedirbutton_clicked()
{
    QDir mdir;
    QString dir = "/New Folder";
    dir = mpath.append(dir);
    if(!QDir(dir).exists())
    {
        mdir.mkpath(dir);
    }
}

void folderbrowserDialog::on_okbutton_clicked()
{
    path = mpath;
    emit (selectedPath(path));
}

