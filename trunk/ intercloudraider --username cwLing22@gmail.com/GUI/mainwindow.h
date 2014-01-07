#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QProgressBar>
#include <QFileInfo>

// QJson
#include "parser.h"

#include "GUI/settingdialog.h"
#include "GUI/logpreview.h"
#include "GUI/operationsdialog.h"
#include "GUI/serviceprovidersdialog.h"
#include "GUI/mylabel.h"
#include "GUI/mylabel2.h"
#include "CoreEngines/enginededuplication.h"
#include "CoreEngines/enginepdp.h"
#include "DBContr/databasecontrollersetting.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    void displayTable();

    void displayTable2();

    void setupBackup();

    void setupRetrieve();

    void displayPDPResult(bool result, QString storage);

    Ui::MainWindow *ui;

    bool success;
    int operationMode;

    // Operation preview:
    MyLabel *label3;
    QFrame *frame5;
    QTreeView *treeView;

    // tokens:
    QStringList *TokenList;
    QStringList *DecodingTokenList;
    QStringList TokenNames;
    QList<int> TokenExpiringTime;

    // backup:
    QList<int> file_idList;
    QStringList LocationList;

    // retrieve:
    int file_id;
    QString folder;
    QString title;
    QStringList ListOfId;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void toUpdateMainWindow();

    void toUpdateMainWindow2();

    void toUpdateMainWindow3();

    void toUpdateFrame5();

    void toDropboxTokenTimer();

    void toGoogleDriveTokenTimer();

    void toSkyDriveTokenTimer();

    void toUpdateMainWindow4();

    void toUpdateMainWindow5();

    void toShowVerBut();
    
private slots:
    void on_StartNewOperationButton_clicked();

    void on_CancelOperationButton_clicked();

    void updateMainWindow();

    void updateMainWindow2();

    void updateMainWindow3();

    void updateMainWindow4();

    void updateMainWindow5();

    void updateFrame5();

    void showVerBut();

    void on_RunButton_clicked();

    void on_actionView_Operation_Log_triggered();

    void on_actionPDP_Verification_triggered();

    void on_actionHelp_triggered();

    void on_actionParameter_Setting_triggered();
};

#endif // MAINWINDOW_H
