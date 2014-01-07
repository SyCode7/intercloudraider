#ifndef FOLDERBROWSERDIALOG_H
#define FOLDERBROWSERDIALOG_H

#include <QDialog>
#include <QtGui>

#include <QDir>

namespace Ui {
class folderbrowserDialog;
}

class folderbrowserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit folderbrowserDialog(QWidget *parent = 0);
    ~folderbrowserDialog();

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
    Ui::folderbrowserDialog *ui;

    QFileSystemModel *dirmodel;
    QString path;
    QString mpath;
};

#endif // FOLDERBROWSERDIALOG_H
