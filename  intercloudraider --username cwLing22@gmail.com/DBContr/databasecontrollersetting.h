#ifndef DATABASECONTROLLERSETTING_H
#define DATABASECONTROLLERSETTING_H

#include <QStringList>

// QtSql
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlRecord>

#include <QtGui>
#include <QDebug>

class DatabaseControllerSetting
{
public:
    DatabaseControllerSetting();
    ~DatabaseControllerSetting();

    // parameters setting:
    bool checkParameterAutoRepair();

    void createParameterAutoRepair(int autoRepair);

    void updateParameterAutoRepair(int autoRepair);

    int retrieveAutoRepair();

    bool checkParameterKeySecret();

    QStringList getKeySecret();

    void createParameterKeySecret(QString key, QString secret,
                                     QString GoogleDriveclient_id,
                                     QString GoogleDriveclient_secret,
                                     QString SkyDriveclient_id,
                                     QString SkyDriveclient_secret);

    void updateParameterKeySecret(QString key, QString secret,
                                     QString GoogleDriveclient_id,
                                     QString GoogleDriveclient_secret,
                                     QString SkyDriveclient_id,
                                     QString SkyDriveclient_secret);

    QStringList retrieveKeySecret();

    bool checkBackupFolder();

    void createBackupFolder(QStringList IdList);

    QStringList retrieveFolderId();

    bool checkPDPtokens();

    void createPDPtokens(QStringList tokens);

    QStringList retrievePDPkeys();

    QStringList retrievePDPtokens();

    void deleteEntry(int i);

    void deletePDPtokens();

    void createPreviousTotalChunks(int d);

    void deletePreviousTotalChunks();

    int retrievePreviousTotalChunks();

    void reorganizeDatabase();
};

#endif // DATABASECONTROLLERSETTING_H
