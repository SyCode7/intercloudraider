#ifndef FILEBROWSERDIALOG_H
#define FILEBROWSERDIALOG_H

// QtGui
#include <QDialog>
#include <QtGui>

// QtCore
#include <QDir>

namespace Ui {
class FileBrowserDialog;
}

class FileBrowserDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FileBrowserDialog(QWidget *parent = 0);
    ~FileBrowserDialog();

    QString getPath()
    {
        return path;
    }

signals:
    void selectedPath(QString path);
    
private slots:
    void on_treeView_clicked(const QModelIndex &index);

    void on_makedirbutton_clicked();

    void on_okbutton_clicked();

private:
    Ui::FileBrowserDialog *ui;

    QFileSystemModel *dirmodel;
    QString path;
    QString mpath;
};

#endif // FILEBROWSERDIALOG_H
