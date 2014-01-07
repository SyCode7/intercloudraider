#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <time.h>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("InterCloud RAIDER Application");
    this->setFixedSize(770, 615);

    ui->frame1->setStyleSheet("background-color: rgb(190, 190, 190)");
    ui->frame2->setStyleSheet("background-color: white");

    label3 = new MyLabel("Start New Operation", this);
    MyLabel2 *label5 = new MyLabel2(1, "InterCloud RAIDer help", this);

    QFont font;
    font.setUnderline(true);

    label3->setFont(font);
    label5->setFont(font);

    ui->label2->setStyleSheet("QLabel{color : red}");
    label3->setStyleSheet("QLabel{color : red}");

    QHBoxLayout *hlayout1 = new QHBoxLayout;
    hlayout1->addWidget(ui->label2);
    hlayout1->addWidget(label3);

    QHBoxLayout *hlayout2 = new QHBoxLayout;
    hlayout2->addWidget(ui->label4);
    hlayout2->addWidget(label5);

    ui->frame3->setLayout(hlayout1);
    ui->frame4->setLayout(hlayout2);

    ui->CancelOperationButton->setEnabled(false);
    ui->RunButton->setEnabled(false);

    DatabaseControllerSetting dbcontr_s;
    if(!dbcontr_s.checkPDPtokens())
    {
        ui->actionPDP_Verification->setEnabled(false);
    }

    connect(this, SIGNAL(toUpdateMainWindow()), this, SLOT(updateMainWindow()));
    connect(this, SIGNAL(toUpdateMainWindow2()), this, SLOT(updateMainWindow2()));
    connect(this, SIGNAL(toUpdateMainWindow3()), this, SLOT(updateMainWindow3()));
    connect(this, SIGNAL(toUpdateFrame5()), this, SLOT(updateFrame5()));
    connect(this, SIGNAL(toUpdateMainWindow4()), this, SLOT(updateMainWindow4()));
    connect(this, SIGNAL(toUpdateMainWindow5()), this, SLOT(updateMainWindow5()));
    connect(this, SIGNAL(toShowVerBut()), this, SLOT(showVerBut()));
    connect(label3, SIGNAL(toUpdateFrame5()), this, SLOT(updateFrame5()));
    connect(label3, SIGNAL(toUpdateMainWindow()), this, SLOT(updateMainWindow()));

    TokenList = new QStringList[4];
    TokenNames << "" << "" << "" << "";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayTable()
{ 
    {
        // backup preview:
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("metadata.db");

        if(db.open())
        {
            QString selectString1 = "SELECT * FROM metadata";

            QSqlQueryModel *model1 = new QSqlQueryModel();
            model1->setQuery(selectString1);
            while (model1->canFetchMore())
                model1->fetchMore();
            int rowCount = model1->rowCount();

            QString selectString2 =
            QString("SELECT Title, Size, Location FROM metadata LIMIT %1 OFFSET %2")
                    .arg(file_idList.size())
                    .arg(rowCount - file_idList.size());

            QSqlQueryModel *model2 = new QSqlQueryModel();
            model2->setQuery(selectString2);

            treeView = new QTreeView;
            treeView->setModel(model2);

            QSqlQuery query(selectString2);
            int LocationNo = query.record().indexOf("Location");

            while(query.next())
            {
                QString Location = query.value(LocationNo).toString();
                LocationList.append(Location);
            }

            db.close();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void MainWindow::displayTable2()
{   
    {
        // retrieve preview:
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("metadata.db");

        if(db.open())
        {
            QString selectString =
            QString("SELECT file_id, Title, Size FROM metadata WHERE file_id = %1")
                    .arg(file_id);

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);

            title = model->record(0).value(1).toString();

            treeView = new QTreeView;
            treeView->setModel(model);

            db.close();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void MainWindow::on_StartNewOperationButton_clicked()
{
    OperationsDialog odialog;
    odialog.exec();
    success = odialog.success;
    operationMode = odialog.operationMode;

    if(operationMode == 1)
    {
        file_idList = odialog.file_idList;
    }
    else if(operationMode == 2)
    {
        file_id = odialog.file_id;
        folder = odialog.folder;
    }

    if(success == true)
    {
        emit toUpdateMainWindow();
        emit toUpdateFrame5();
    }
}

void MainWindow::on_CancelOperationButton_clicked()
{
    if(operationMode == 1)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Cancel Operation",
                                      "Are you sure you want to cancel the Backup?",
                              QMessageBox::Ok|QMessageBox::Cancel);
        if(reply == QMessageBox::Ok)
        {
            emit toUpdateMainWindow2();

            LocationList.clear();

            DatabaseControllerMetadata dbcontr;
            dbcontr.deleteEntries(file_idList);
        }

    }
    else if(operationMode == 2)
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Cancel Operation",
                                      "Are you sure you want to cancel the Retrieve?",
                              QMessageBox::Ok|QMessageBox::Cancel);
        if(reply == QMessageBox::Ok)
        {
            emit toUpdateMainWindow2();
        }
    }
}

void MainWindow::updateMainWindow()
{
    ui->label1->hide();
    ui->frame3->hide();
    ui->frame4->hide();

    ui->CancelOperationButton->setEnabled(true);
    ui->RunButton->setEnabled(true);

    ui->StartNewOperationButton->setEnabled(false);
    ui->actionParameter_Setting->setEnabled(false);
    ui->actionView_Operation_Log->setEnabled(false);
    ui->actionPDP_Verification->setEnabled(false);
}

void MainWindow::updateMainWindow2()
{
    ui->label1->show();
    ui->frame3->show();
    ui->frame4->show();

    ui->StartNewOperationButton->setEnabled(true);
    ui->actionParameter_Setting->setEnabled(true);
    ui->actionView_Operation_Log->setEnabled(true);

    emit toShowVerBut();

    ui->CancelOperationButton->setEnabled(false);
    ui->RunButton->setEnabled(false);

    delete frame5;
}

void MainWindow::updateMainWindow3()
{
    delete frame5;

    ui->label1->show();
    ui->frame3->show();
    ui->frame4->show();

    ui->StartNewOperationButton->setEnabled(true);
    ui->actionParameter_Setting->setEnabled(true);
    ui->actionView_Operation_Log->setEnabled(true);

    emit toShowVerBut();
}

void MainWindow::updateMainWindow4()
{
    ui->frame3->setEnabled(false);

    ui->StartNewOperationButton->setEnabled(false);
    ui->actionParameter_Setting->setEnabled(false);
    ui->actionView_Operation_Log->setEnabled(false);
    ui->actionPDP_Verification->setEnabled(false);
}

void MainWindow::updateMainWindow5()
{
    ui->frame3->setEnabled(true);

    ui->StartNewOperationButton->setEnabled(true);
    ui->actionParameter_Setting->setEnabled(true);
    ui->actionView_Operation_Log->setEnabled(true);

    emit toShowVerBut();
}

void MainWindow::updateFrame5()
{
    frame5 = new QFrame(ui->frame2);

    QFont font;
    font.setBold(true);

    QVBoxLayout *mainlayout = new QVBoxLayout;
    QLabel *label6 = new QLabel(tr("Selected Operation: "));

    QGridLayout *glayout = new QGridLayout;
    QGridLayout *glayout2 = new QGridLayout;

    if(label3->success == true)
    {
        operationMode = label3->operationMode;

        if(operationMode == 1)
        {
            file_idList = label3->file_idList;
        }
        else if(operationMode == 2)
        {
            file_id = label3->file_id;
            folder = label3->folder;
        }

        label3->success = false;
    }

    if(operationMode == 1)
    {
        QLabel *label7 = new QLabel(tr("Backup"));
        label7->setFont(font);
        QLabel *label8 = new QLabel(tr("Selected Files:"));

        glayout->addWidget(label6, 0, 0);
        glayout->addWidget(label7, 0, 1);
        glayout->addWidget(label8, 1, 0);

        // for encoding preview:
        displayTable();

        mainlayout->addLayout(glayout);
        mainlayout->addWidget(treeView);

        frame5->setLayout(mainlayout);
        frame5->setGeometry(0, 0, 620, 400);
        frame5->show();
    }

    else if(operationMode == 2)
    {
        QLabel *label7 = new QLabel(tr("Retrieve"));
        label7->setFont(font);
        QLabel *label8 = new QLabel(tr("Selected File:"));
        QLabel *label9 = new QLabel(tr("Selected Folder:"));
        QLabel *label10 = new QLabel(folder);
        label10->setFont(font);

        glayout->addWidget(label6, 0, 0);
        glayout->addWidget(label7, 0, 1);
        glayout->addWidget(label8, 1, 0);

        glayout2->addWidget(label9, 0, 0);
        glayout2->addWidget(label10, 0, 1);

        // for decoding preview:
        displayTable2();

        mainlayout->addLayout(glayout);
        mainlayout->addWidget(treeView);
        mainlayout->addLayout(glayout2);

        frame5->setLayout(mainlayout);
        frame5->setGeometry(0, 0, 620, 400);
        frame5->show();
    }
}

void MainWindow::showVerBut()
{
    DatabaseControllerSetting dbcontr_s;
    if(dbcontr_s.checkPDPtokens())
    {
        ui->actionPDP_Verification->setEnabled(true);
    }
}

void MainWindow::setupBackup()
{
    // check whether the TokenList exists. If yes, continue to encoding step;
    // if nope, first generate a TokenList by starting up OAuth/OAuth2 authentication,
    // and then continue to encoding step.
    if(TokenNames.contains("Dropbox") &&
       TokenNames.contains("Google Drive") &&
       TokenNames.contains("SkyDrive") &&
       TokenNames.contains("Ubuntu"))
    {
        // create folder backup at cloud storage providers, if not exist:
        DatabaseControllerSetting dbcontr_s;
        if(!dbcontr_s.checkBackupFolder())
        {
            QStringList IdList;

            NetworkControllerParallel controller;
            IdList = controller.createBackup(TokenList);
            dbcontr_s.createBackupFolder(IdList);
        }
    }
    else
    {
        // do login:
        QStringList *EncodingTokenList = new QStringList;
        NetworkControllerParallel controller;
        EncodingTokenList = controller.startUploadSession(TokenNames, TokenList);

        // update backgroud TokenList:
        if(controller.Dropbox.contains("Dropbox"))
        {
            TokenNames[0] = "Dropbox";
            TokenList[0] = EncodingTokenList[0];
        }

        if(controller.GoogleDrive.contains("Google Drive"))
        {
            TokenNames[1] = "Google Drive";
            TokenList[1] = EncodingTokenList[1];
        }

        if(controller.SkyDrive.contains("SkyDrive"))
        {
            TokenNames[2] = "SkyDrive";
            TokenList[2] = EncodingTokenList[2];
        }
        if(controller.Ubuntu.contains("Ubuntu"))
        {
            TokenNames[3] = "Ubuntu";
            TokenList[3] = EncodingTokenList[3];
        }

        // create folder backup at cloud storage providers, if not exist:
        DatabaseControllerSetting dbcontr_s;
        if(!dbcontr_s.checkBackupFolder())
        {
            QStringList IdList;
            IdList = controller.createBackup(TokenList);
            dbcontr_s.createBackupFolder(IdList);
        }
    }
}

void MainWindow::setupRetrieve()
{
    // Select cloud storage provider(s) for decoding.
    QStringList providers;

    ServiceProvidersDialog *sdialog = new ServiceProvidersDialog(this);
    (*sdialog).setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    (*sdialog).exec();
    (*sdialog).setFixedSize(240, 160);
    providers = (*sdialog).provider;

    // do login:
    DecodingTokenList = new QStringList;
    NetworkControllerParallel controller;
    DecodingTokenList = controller.startDownloadSession(providers, TokenNames, TokenList);

    // update backgroud TokenList:
    if(controller.Dropbox.contains("Dropbox"))
    {
        TokenNames[0] = "Dropbox";
        TokenList[0] = DecodingTokenList[0];
    }

    if(controller.GoogleDrive.contains("Google Drive"))
    {
        TokenNames[1] = "Google Drive";
        TokenList[1] = DecodingTokenList[0];
    }

    if(controller.SkyDrive.contains("SkyDrive"))
    {
        TokenNames[2] = "SkyDrive";
        TokenList[2] = DecodingTokenList[0];
    }

    if(controller.Ubuntu.contains("Ubuntu"))
    {
        TokenNames[3] = "Ubuntu";
        TokenList[3] = DecodingTokenList[1];
    }
}

void MainWindow::on_RunButton_clicked()
{
    ui->StartNewOperationButton->setEnabled(false);
    ui->CancelOperationButton->setEnabled(false);
    ui->RunButton->setEnabled(false);

    if(operationMode == 1)
    {   
        // Start backup:
        setupBackup();
        EngineDeduplication backup;

        for(int i = 0; i < LocationList.size(); i++)
        {
            QFileInfo fileInfo(LocationList.at(i));
            backup.BackupAlgorithm(fileInfo.absoluteFilePath(),
                                   file_idList.at(i), TokenList);
        }

        if(backup.getIsChanged())
        {
            TokenList = backup.getEncodingTokenList();
        }

        LocationList.clear();

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "PDP Setup",
                                      "Do you want to generate new verification tokens?",
                              QMessageBox::Ok|QMessageBox::Cancel);
        if(reply == QMessageBox::Ok)
        {
            DatabaseControllerSetting dbcontr_s;
            if(dbcontr_s.checkPDPtokens())
            {
                dbcontr_s.deletePDPtokens();
                dbcontr_s.reorganizeDatabase();
            }

            // Generate PDP tokens:
            EnginePDP pdp;
            QStringList tokens = pdp.PDPSetup();
            dbcontr_s.createPDPtokens(tokens);
        }

        emit toUpdateMainWindow3();
    }

    else if(operationMode == 2)
    {
        // Start retrieve:
        setupRetrieve();
        EngineDeduplication retrieve;
        retrieve.RetrieveAlgorithm(file_id, title, folder, DecodingTokenList);

        if(retrieve.getIsChanged())
        {
            DecodingTokenList = retrieve.getDecodingTokenList();

            if(DecodingTokenList[0].contains("Google Drive"))
            {
                TokenList[1] = DecodingTokenList[0];
            }
            else if(DecodingTokenList[0].contains("SkyDrive"))
            {
                TokenList[2] = DecodingTokenList[0];
            }
        }

        LocationList.clear();

        emit toUpdateMainWindow3();
    }
}

void MainWindow::on_actionView_Operation_Log_triggered()
{
    LogPreview *preview = new LogPreview(TokenNames, TokenList, this);
    connect(preview, SIGNAL(started()), this, SLOT(updateMainWindow4()));
    connect(preview, SIGNAL(finished()), this, SLOT(updateMainWindow5()));
    (*preview).exec();

    TokenNames = (*preview).TokenNames;
    TokenList = (*preview).TokenList;
}

void MainWindow::displayPDPResult(bool result, QString storage)
{
    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::WindowTitleHint);
    msgBox.setWindowTitle("PDP Verification Result");
    if(result == true)
        msgBox.setText(QString("Data at %1 is intact.      ").arg(storage));
    else
        msgBox.setText(QString("Data at %1 has been modified or lost!      ")
                       .arg(storage));
    msgBox.exec();
}

void MainWindow::on_actionPDP_Verification_triggered()
{
    emit toUpdateMainWindow4();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "PDP Verification",
                                  "Do you want to verify your data is in possession?",
                          QMessageBox::Ok|QMessageBox::Cancel);
    if(reply == QMessageBox::Ok)
    {
        setupRetrieve();
        QString storage;
        if(DecodingTokenList[0].contains("Dropbox"))
        {
            storage = "Dropbox";
        }
        else if(DecodingTokenList[0].contains("Google Drive"))
        {
            storage = "Google Drive";
        }
        else if(DecodingTokenList[0].contains("Ubuntu"))
        {
            storage = "Ubuntu One";
        }

        DatabaseControllerSetting dbcontr_s;
        QStringList tokens = dbcontr_s.retrievePDPtokens();
        EnginePDP pdp;
        int i = pdp.TOTAL_TOKENS - tokens.size();

        if(tokens.size() == 1)
        {
            if(pdp.PDPVerification(DecodingTokenList, i))
            {
                displayPDPResult(true, storage);
            }
            else
            {
                displayPDPResult(false, storage);
            }

            dbcontr_s.deletePDPtokens();
            dbcontr_s.reorganizeDatabase();
            tokens.clear();

            // Regenerate PDP tokens, since the last token is used up for checking:
            tokens = pdp.PDPSetup();
            dbcontr_s.createPDPtokens(tokens);
        }
        else
        {
            if(pdp.PDPVerification(DecodingTokenList, i))
            {
                displayPDPResult(true, storage);
            }
            else
            {             
                displayPDPResult(false, storage);
            }

            dbcontr_s.deleteEntry(i);
        }

        if(pdp.getIsChanged())
        {
            DecodingTokenList = pdp.getDecodingTokenList();

            if(DecodingTokenList[0].contains("Google Drive"))
            {
                TokenList[1] = DecodingTokenList[0];
            }
            else if(DecodingTokenList[0].contains("SkyDrive"))
            {
                TokenList[2] = DecodingTokenList[0];
            }
        }
    }

    emit toUpdateMainWindow5();
}

void MainWindow::on_actionHelp_triggered()
{
    QString uri = "https://code.google.com/p/intercloudraider/";
    QUrl url = uri;
    QDesktopServices::openUrl(url);
}

void MainWindow::on_actionParameter_Setting_triggered()
{
    SettingDialog setting;
    setting.exec();
}
