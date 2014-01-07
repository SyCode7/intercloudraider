#ifndef LOGPREVIEW_H
#define LOGPREVIEW_H

#include <QDialog>
#include <QProgressDialog>
#include <QtSql>

#include "CoreEngines/enginedelete.h"

namespace Ui {
class LogPreview;
}

class LogPreview : public QDialog
{
    Q_OBJECT

    Ui::LogPreview *ui;

    void displayTable();

    void setupTokenList();

    QSqlQueryModel *model;

    int file_id;
    QString filename;
    
public:
    explicit LogPreview(QStringList tokenNames, QStringList *tokenList,
                        QWidget *parent = 0);
    ~LogPreview();

    QStringList *TokenList;
    QStringList TokenNames;

    QString Dropbox;
    QString GoogleDrive;
    QString SkyDrive;
    QString Ubuntu;

signals:
    void started();

    void finished();
    
private slots:
    void on_treeView_clicked(const QModelIndex &index);

    void on_deleteButton_clicked();

    void on_closeButton_clicked();
};

#endif // LOGPREVIEW_H
