#include "databasecontrollersetting.h"
#include "ui_keysecretdialog.h"

DatabaseControllerSetting::DatabaseControllerSetting()
{
}

DatabaseControllerSetting::~DatabaseControllerSetting()
{
}

// Check wheter the table parameterAutoRepair exists.
bool DatabaseControllerSetting::checkParameterAutoRepair()
{
    bool result;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");


        if(db.open())
        {
            QStringList tablelist;
            tablelist = db.tables(QSql::Tables);

            if(tablelist.contains("parameterAutoRepair"))
            {
                db.close();
                result = true;
            }
            else
            {
                db.close();
                result = false;
            }
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            result = checkParameterAutoRepair();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return result;
}

// Create the table parameterAutorepair.
void DatabaseControllerSetting::createParameterAutoRepair(int autoRepair)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QString createString = "CREATE TABLE IF NOT EXISTS parameterAutoRepair";
            createString.append("(id INTEGER PRIMARY KEY, ");
            createString.append("autoRepair INTEGER NOT NULL)");

            QSqlQuery query;
            if (!query.exec(createString))
            {
                qDebug() <<  "Error = " << query.lastError();
            }
            else
            {
                QString insertString = "INSERT INTO parameterAutoRepair";
                insertString.append("(id, ");
                insertString.append("autoRepair)");
                insertString.append("VALUES(:id, ");
                insertString.append(":autoRepair)");

                query.prepare(insertString);
                query.bindValue(":autoRepair", autoRepair);

                if (!query.exec())
                {
                    qDebug() <<  "Error = " << query.lastError();
                }
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

// Update parameterAutoRepair.
void DatabaseControllerSetting::updateParameterAutoRepair(int autoRepair)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QString updateString =
            QString("UPDATE parameterAutoRepair SET autoRepair = %1").arg(autoRepair);

            QSqlQuery query;
            query.prepare(updateString);

            if (!query.exec())
            {
                qDebug() <<  "Error = " << query.lastError();
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

// Retrieve the information from parameterAutoRepair.
int DatabaseControllerSetting::retrieveAutoRepair()
{
    int autoRepair;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QString selectString =
            QString("SELECT autoRepair FROM parameterAutoRepair");

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);

            autoRepair = model->record(0).value(0).toInt();

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            autoRepair = retrieveAutoRepair();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return autoRepair;
}

// Check whether the table keys_secrets exists.
bool DatabaseControllerSetting::checkParameterKeySecret()
{
    bool result;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QStringList tablelist;
            tablelist = db.tables(QSql::Tables);

            if(tablelist.contains("keys_secrets"))
            {
                db.close();
                result = true;
            }
            else
            {
                db.close();
                result = false;
            }
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            result = checkParameterKeySecret();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return result;
}

// Create key and secret pairs.
QStringList DatabaseControllerSetting::getKeySecret()
{
    QStringList key_secret;

    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    msgBox.setWindowTitle("Application Registering Step 1: Dropbox");
    msgBox.setText("Please visit the following link and follow the instructions "
                   "to register for an application account in order to get "
                   "key and secret pair:");
    QPushButton *okbutton = msgBox.addButton(QString("Open Link"), QMessageBox::YesRole);
    msgBox.exec();

    if(msgBox.clickedButton() == okbutton)
    {
        QUrl dropbox = "https://www.dropbox.com/developers/apps";
        QDesktopServices::openUrl(dropbox);

        QDialog ksdialog;
        ksdialog.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
        Ui::Dialog ui;
        ui.setupUi(&ksdialog);
        ksdialog.setWindowTitle("Dropbox Key And Secret Input");
        ksdialog.resize(390, 130);
        ui.label->setText("key   :");
        ui.label_2->setText("secret:");

        if(ksdialog.exec() == QDialog::Accepted)
        {
            key_secret.append(ui.lineEdit->text());
            key_secret.append(ui.lineEdit_2->text());

            ksdialog.deleteLater();
        }
    }

    QMessageBox msgBox2;
    msgBox2.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    msgBox2.setWindowTitle("Application Registering Step 2: Google Drive");
    msgBox2.setText("Please visit the following link and follow the instructions "
                   "to register for an application account:");
    QPushButton *okbutton2 = msgBox2.addButton(QString("Open Link"), QMessageBox::YesRole);
    msgBox2.exec();

    if(msgBox2.clickedButton() == okbutton2)
    {
        QUrl googledrive = "https://developers.google.com/drive/enable-sdk";
        QDesktopServices::openUrl(googledrive);

        QDialog ksdialog;
        ksdialog.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
        Ui::Dialog ui;
        ui.setupUi(&ksdialog);
        ksdialog.setWindowTitle("Google Drive Client ID And Secret Input");
        ksdialog.resize(390, 130);
        ui.label->setText("Client ID      :");
        ui.label_2->setText("Client secret:");

        if(ksdialog.exec() == QDialog::Accepted)
        {
            key_secret.append(ui.lineEdit->text());
            key_secret.append(ui.lineEdit_2->text());

            ksdialog.deleteLater();
        }
    }

    QMessageBox msgBox3;
    msgBox3.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    msgBox3.setWindowTitle("Application Registering Step 3: SkyDrive");
    msgBox3.setText("Please visit the following link and follow the instructions "
                   "to register for an application account:");
    QPushButton *okbutton3 = msgBox3.addButton(QString("Open Link"), QMessageBox::YesRole);
    msgBox3.exec();

    if(msgBox3.clickedButton() == okbutton3)
    {
        QUrl skydrive = "https://account.live.com/developers/applications";
        QDesktopServices::openUrl(skydrive);

        QDialog ksdialog;
        ksdialog.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
        Ui::Dialog ui;
        ui.setupUi(&ksdialog);
        ksdialog.setWindowTitle("SkyDrive Client ID And Secret Input");
        ksdialog.resize(390, 130);
        ui.label->setText("Client ID      :");
        ui.label_2->setText("Client secret:");

        if(ksdialog.exec() == QDialog::Accepted)
        {
            key_secret.append(ui.lineEdit->text());
            key_secret.append(ui.lineEdit_2->text());

            ksdialog.deleteLater();
        }
    }

    return key_secret;
}


// Create the table key_secret.
void DatabaseControllerSetting::createParameterKeySecret(QString key, QString secret, QString GoogleDriveclient_id, QString GoogleDriveclient_secret, QString SkyDriveclient_id, QString SkyDriveclient_secret)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QString createString = "CREATE TABLE IF NOT EXISTS keys_secrets";
            createString.append("(id INTEGER PRIMARY KEY, ");
            createString.append("key TEXT NOT NULL, ");
            createString.append("secret TEXT NOT NULL, ");
            createString.append("GoogleDriveclient_id TEXT NOT NULL, ");
            createString.append("GoogleDriveclient_secret TEXT NOT NULL, ");
            createString.append("SkyDriveclient_id TEXT NOT NULL, ");
            createString.append("SkyDriveclient_secret TEXT NOT NULL)");

            QSqlQuery query;
            if (!query.exec(createString))
            {
                qDebug() <<  "Error = " << query.lastError();
            }
            else
            {
                QString insertString = "INSERT INTO keys_secrets";
                insertString.append("(id, ");
                insertString.append("key, ");
                insertString.append("secret, ");
                insertString.append("GoogleDriveclient_id, ");
                insertString.append("GoogleDriveclient_secret, ");
                insertString.append("SkyDriveclient_id, ");
                insertString.append("SkyDriveclient_secret)");

                insertString.append("VALUES(:id, ");
                insertString.append(":key, ");
                insertString.append(":secret, ");
                insertString.append(":GoogleDriveclient_id, ");
                insertString.append(":GoogleDriveclient_secret, ");
                insertString.append(":SkyDriveclient_id, ");
                insertString.append(":SkyDriveclient_secret)");

                query.prepare(insertString);
                query.bindValue(":key", key);
                query.bindValue(":secret", secret);
                query.bindValue(":GoogleDriveclient_id", GoogleDriveclient_id);
                query.bindValue(":GoogleDriveclient_secret", GoogleDriveclient_secret);
                query.bindValue(":SkyDriveclient_id", SkyDriveclient_id);
                query.bindValue(":SkyDriveclient_secret", SkyDriveclient_secret);

                if (!query.exec())
                {
                    qDebug() <<  "Error = " << query.lastError();
                }
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

// Update keys_secrets.
void DatabaseControllerSetting::updateParameterKeySecret(QString key, QString secret, QString GoogleDriveclient_id, QString GoogleDriveclient_secret, QString SkyDriveclient_id, QString SkyDriveclient_secret)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QString updateString =
                    QString("UPDATE keys_secrets SET key = \'%1\', secret = \'%2\',"
                            "GoogleDriveclient_id = \'%3\', GoogleDriveclient_secret = \'%4\',"
                            "SkyDriveclient_id = \'%5\', SkyDriveclient_secret = \'%6\'")
                            .arg(key).arg(secret)
                            .arg(GoogleDriveclient_id).arg(GoogleDriveclient_secret)
                            .arg(SkyDriveclient_id).arg(SkyDriveclient_secret);

            QSqlQuery query;
            query.prepare(updateString);

            if (!query.exec())
            {
                qDebug() <<  "Error = " << query.lastError();
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

// Retrieve the information from keys_secrets.
QStringList DatabaseControllerSetting::retrieveKeySecret()
{
    QStringList key_secret;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QString selectString =
            QString("SELECT * FROM keys_secrets");

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);

            for(int i = 1; i < model->columnCount(); i++)
            {
                key_secret.append(model->record(0).value(i).toString());
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            key_secret = retrieveKeySecret();
        }


    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return key_secret;
}

// Check whether the table BackupFolder exists.
bool DatabaseControllerSetting::checkBackupFolder()
{
    bool result;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QStringList tablelist;
            tablelist = db.tables(QSql::Tables);

            if(tablelist.contains("BackupFolder"))
            {
                db.close();
                result = true;
            }
            else
            {
                db.close();
                result = false;
            }
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            result = checkBackupFolder();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return result;
}

// Create the table BackupFolder.
void DatabaseControllerSetting::createBackupFolder(QStringList IdList)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        // Create folder backup at Google Drive, Skydrive, and UbuntuOne.
        // Dropbox backup folder is not included:
        if(db.open())
        {
            QString createString = "CREATE TABLE IF NOT EXISTS BackupFolder";
            createString.append("(id INTEGER PRIMARY KEY, ");
            createString.append("folder_id2 TEXT NOT NULL, ");
            createString.append("folder_id3 TEXT NOT NULL, ");
            createString.append("folder_id4 TEXT NOT NULL)");

            QSqlQuery query;
            if (!query.exec(createString))
            {
                qDebug() <<  "Error = " << query.lastError();
            }
            else
            {
                QString insertString = "INSERT INTO BackupFolder";
                insertString.append("(id, ");
                insertString.append("folder_id2, ");
                insertString.append("folder_id3, ");
                insertString.append("folder_id4)");
                insertString.append("VALUES(:id, ");
                insertString.append(":folder_id2, ");
                insertString.append(":folder_id3, ");
                insertString.append(":folder_id4)");

                query.prepare(insertString);
                query.bindValue(":folder_id2", IdList[0]);
                query.bindValue(":folder_id3", IdList[1]);
                query.bindValue(":folder_id4", IdList[2]);

                if (!query.exec())
                {
                    qDebug() <<  "Error = " << query.lastError();
                }
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

// Retrieve information from BackupFolder.
QStringList DatabaseControllerSetting::retrieveFolderId()
{
    QStringList folder_id;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QString selectString =
            QString("SELECT * FROM BackupFolder");

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);

            for(int i = 1; i < model->columnCount(); i++)
            {
                folder_id.append(model->record(0).value(i).toString());
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            folder_id = retrieveFolderId();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return folder_id;
}

// Check wheter the table PDPtokens exists.
bool DatabaseControllerSetting::checkPDPtokens()
{
    bool result;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");


        if(db.open())
        {
            QStringList tablelist;
            tablelist = db.tables(QSql::Tables);

            if(tablelist.contains("PDPtokens"))
            {
                db.close();
                result = true;
            }
            else
            {
                db.close();
                result = false;
            }
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            result = checkPDPtokens();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return result;
}

// Create the table PDPtokens.
void DatabaseControllerSetting::createPDPtokens(QStringList tokens)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QString createString = "CREATE TABLE IF NOT EXISTS PDPtokens";
            createString.append("(id INTEGER PRIMARY KEY, ");
            createString.append("token TEXT NOT NULL)");

            QSqlQuery query;
            if (!query.exec(createString))
            {
                qDebug() <<  "Error = " << query.lastError();
            }
            else
            {
                QString insertString = "INSERT INTO PDPtokens(token)";
                insertString.append("VALUES(?)");
                query.prepare(insertString);
                query.addBindValue(tokens);
                if (!query.execBatch())
                {
                    qDebug() <<  "Error = " << query.lastError();
                }
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

QStringList DatabaseControllerSetting::retrievePDPkeys()
{
    QStringList InfoList;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QString selectString = QString("SELECT * FROM PDPtokens");

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);

            // keys are i = 0 and i = 1:
            for(int i = 0; i < 2; i++)
            {
                InfoList.append(model->record(i).value(1).toString());
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            InfoList = retrievePDPkeys();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return InfoList;
}

QStringList DatabaseControllerSetting::retrievePDPtokens()
{
    QStringList InfoList;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QString selectString = QString("SELECT * FROM PDPtokens");

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);

            // tokens start from i = 2:
            for(int i = 2; i < model->rowCount(); i++)
            {
                InfoList.append(model->record(i).value(1).toString());
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            InfoList = retrievePDPtokens();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return InfoList;
}

// Remove one entry from the table PDPtokens.
void DatabaseControllerSetting::deleteEntry(int i)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QString deleteString = QString("DELETE FROM PDPtokens WHERE id = %1")
                    .arg(3 + i);

            QSqlQuery query;
            if (!query.exec(deleteString))
            {
                qDebug() <<  "Error = " << query.lastError();
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

// Delete the table PDPtokens.
void DatabaseControllerSetting::deletePDPtokens()
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QString dropString = QString("DROP TABLE IF EXISTS PDPtokens");

            QSqlQuery query;
            if (!query.exec(dropString))
            {
                qDebug() <<  "Error = " << query.lastError();
            }
            else
            {
                qDebug() << QString("The table PDPtokens is removed");
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

// Create the table previousTotalChunks.
void DatabaseControllerSetting::createPreviousTotalChunks(int d)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QString createString = "CREATE TABLE IF NOT EXISTS PreviousTotalChunks";
            createString.append("(id INTEGER PRIMARY KEY, ");
            createString.append("totalchunks INTEGER NOT NULL)");

            QSqlQuery query;
            if (!query.exec(createString))
            {
                qDebug() <<  "Error = " << query.lastError();
            }
            else
            {
                QString insertString = "INSERT INTO PreviousTotalChunks";
                insertString.append("(id, ");
                insertString.append("totalchunks)");
                insertString.append("VALUES(:id, ");
                insertString.append(":totalchunks)");

                query.prepare(insertString);
                query.bindValue(":totalchunks", d);

                if (!query.exec())
                {
                    qDebug() <<  "Error = " << query.lastError();
                }
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

// Delete the table previousTotalChunks.
void DatabaseControllerSetting::deletePreviousTotalChunks()
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QString dropString = QString("DROP TABLE IF EXISTS PreviousTotalChunks");

            QSqlQuery query;
            if (!query.exec(dropString))
            {
                qDebug() <<  "Error = " << query.lastError();
            }
            else
            {
                qDebug() << QString("The table PreviousTotalChunks is removed");
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

// Retrieve totalchunks from previousTotalChunks.
int DatabaseControllerSetting::retrievePreviousTotalChunks()
{
    int d;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QString selectString =
            QString("SELECT totalchunks FROM PreviousTotalChunks");

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);

            d = model->record(0).value(0).toInt();

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            d = retrievePreviousTotalChunks();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return d;
}

// Reorganize the database setting.
void DatabaseControllerSetting::reorganizeDatabase()
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("setting.db");

        if(db.open())
        {
            QString vacuumString = QString("VACUUM");

            QSqlQuery query;
            if (!query.exec(vacuumString))
            {
                qDebug() <<  "Error = " << query.lastError();
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}
