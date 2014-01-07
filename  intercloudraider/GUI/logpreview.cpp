#include "logpreview.h"
#include "ui_logpreview.h"

LogPreview::LogPreview(QStringList tokenNames, QStringList *tokenList,
                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogPreview)
{
    ui->setupUi(this);

    setWindowTitle("Operation Log");

    ui->deleteButton->setEnabled(false);

    file_id = 0; // file_id counted from 1
    TokenNames = tokenNames;
    TokenList = tokenList;

    displayTable();
}

LogPreview::~LogPreview()
{
    delete ui;
}

void LogPreview::displayTable()
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("metadata.db");

        if(db.open())
        {
            model = new QSqlQueryModel();
            model->setQuery("SELECT * FROM metadata");
            while (model->canFetchMore())
                model->fetchMore();

            ui->treeView->setModel(model);

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void LogPreview::setupTokenList()
{
    // do login:
    QStringList *mTokenList = new QStringList;
    NetworkControllerParallel nwcontroller;
    mTokenList = nwcontroller.startUploadSession(TokenNames, TokenList);

    // update backgroud TokenList:
    if(nwcontroller.Dropbox.contains("Dropbox"))
    {
        Dropbox = "Dropbox";
        TokenNames[0] = "Dropbox";
        TokenList[0] = mTokenList[0];
    }

    if(nwcontroller.GoogleDrive.contains("Google Drive"))
    {
        GoogleDrive = "GoogleDrive";
        TokenNames[1] = "Google Drive";
        TokenList[1] = mTokenList[1];
    }

    if(nwcontroller.SkyDrive.contains("SkyDrive"))
    {
        SkyDrive = "SkyDrive";
        TokenNames[2] = "SkyDrive";
        TokenList[2] = mTokenList[2];
    }

    if(nwcontroller.Ubuntu.contains("Ubuntu"))
    {
        Ubuntu = "Ubuntu";
        TokenNames[3] = "Ubuntu";
        TokenList[3] = mTokenList[3];
    }
}

void LogPreview::on_treeView_clicked(const QModelIndex &index)
{
    file_id = model->record(index.row()).value(0).toInt();
    filename = model->record(index.row()).value(1).toString();

    ui->deleteButton->setEnabled(true);
}

void LogPreview::on_deleteButton_clicked()
{
    emit started();

    if(TokenNames.contains("Dropbox") &&
       TokenNames.contains("Google Drive") &&
       TokenNames.contains("SkyDrive") &&
       TokenNames.contains("Ubuntu"))
    {
        // start delete:
        EngineDelete deleting;
        deleting.deleteRecord(file_id, TokenList);

        if(deleting.getIsChanged())
            TokenList = deleting.getTokenList();
    }
    else
    {
        setupTokenList();

        // start delete:
        EngineDelete deleting;
        deleting.deleteRecord(file_id, TokenList);

        if(deleting.getIsChanged())
            TokenList = deleting.getTokenList();
    }

    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::WindowTitleHint);
    msgBox.setWindowTitle("Remove File");
    msgBox.setText(QString("%1 deleted!      ").arg(filename));
    msgBox.exec();

    emit finished();
}

void LogPreview::on_closeButton_clicked()
{
    this->close();
}
