#ifndef HSRCENCODINGDIALOG_H
#define HSRCENCODINGDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QDebug>

#include "GUI/filebrowserdialog.h"
#include "DBContr/databasecontrollersetting.h"
#include "DBContr/databasecontrollermetadata.h"

namespace Ui {
class HSRCEncodingDialog;
}

class HSRCEncodingDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit HSRCEncodingDialog(QWidget *parent = 0);
    ~HSRCEncodingDialog();

    bool success; // return success if the metadata is inserted
    QList<int> file_idList;
    
private slots:
    void on_inputbrowse_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    int pathIsDir(QString path);

    Ui::HSRCEncodingDialog *ui;

    QString inputpath;
    QFileInfoList filelist; // same as QList<QFileInfo>
};

#endif // HSRCENCODINGDIALOG_H
