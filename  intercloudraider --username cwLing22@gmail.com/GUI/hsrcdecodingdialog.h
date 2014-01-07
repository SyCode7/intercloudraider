#ifndef HSRCDECODINGDIALOG_H
#define HSRCDECODINGDIALOG_H

#include <QDialog>
#include <QMessageBox>

// QtSql
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlRecord>

#include "GUI/folderbrowserdialog.h"

namespace Ui {
class HSRCDecodingDialog;
}

class HSRCDecodingDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit HSRCDecodingDialog(QWidget *parent = 0);
    ~HSRCDecodingDialog();

    bool success; // return success if an encoded file with file_id is selected
    int file_id;
    QString folder;
    
private slots:
    void on_treeView_clicked(const QModelIndex &index);

    void on_outputbrowse_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    void displayTable();

    Ui::HSRCDecodingDialog *ui;

    QSqlDatabase db;
    QSqlQueryModel *model;
};

#endif // HSRCDECODINGDIALOG_H
