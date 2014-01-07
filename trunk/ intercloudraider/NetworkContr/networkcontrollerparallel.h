#ifndef NETWORKCONTROLLERPARALLEL_H
#define NETWORKCONTROLLERPARALLEL_H

#include <QObject>
#include <QtNetwork>
#include <QEventLoop>

#include <time.h>

#include "NetworkContr/sessiondropbox.h"
#include "NetworkContr/sessiongoogledrive.h"
#include "NetworkContr/sessionskydrive.h"
#include "NetworkContr/sessionubuntuone.h"

// QJson
#include "parser.h"
#include "CoreEngines/enginecryptographichashing.h"

class NetworkControllerParallel : public QObject
{
    Q_OBJECT

    QStringList blockname; // there are 6 blocknames
    QStringList *mTokenList;
    int size, N, mK;
    unsigned char **encoded_parts;

    QString Dropbox_upload_uri, Dropbox_download_uri,
            Dropbox_delete_uri, Dropbox_metadata_uri,
            Dropbox_key, Dropbox_secret;

    QString GoogleDrive_upload_uri, GoogleDrive_download_uri,
            GoogleDrive_folder_uri;
    QStringList folder_id;

    QString base_uri, file_uri,
            api, volume,
            Ubuntu_key, Ubuntu_secret;

    QString SkyDrive_upload_uri, SkyDrive_download_uri,
            SkyDrive_folder_uri;

    int ctr;

    QStringList mListOfId;
    QStringList *dTokenList;

    int dctr;

    int dectr;

    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    QNetworkReply *reply1;
    QNetworkReply *reply2;
    QNetworkReply *reply3;
    QNetworkReply *reply4;
    QNetworkReply *reply5;
    QNetworkReply *reply6;

    QUrl setDropboxUrl(QString, QString, QStringList);
    QUrl setGoogleUrl(QString, QStringList);
    QByteArray setGoogleMetadata(QString);
    QNetworkRequest setGoogleRequest(QUrl, int, int);
    QUrl setUbuntuUrl(QString, QString, QStringList);

    void DropboxUpload(QString, int, unsigned char*, QStringList);
    void DropboxUpload2(QString, int, unsigned char*, QStringList);
    void GoogleDriveUpload(QString, int, QStringList);
    void GoogleDriveUpload2(QString, int, QStringList);
    void SkyDriveUpload(QString, int, unsigned char*, QStringList);
    void UbuntuOneUpload(QString, QStringList);

    void DropboxDownload(QString, QStringList);
    void DropboxDownload2(QString, QStringList);
    void GoogleDriveDownload(QString, QStringList);
    void GoogleDriveDownload2(QString, QStringList);
    void SkyDriveDownload(QString, QStringList);
    void DownloadLoop(QUrl);
    void UbuntuOneDownload(QString, QStringList);

    void DropboxDelete(QString, QStringList);
    void DropboxDelete2(QString, QStringList);
    void GoogleDriveDelete(QString, QStringList);
    void GoogleDriveDelete2(QString, QStringList);
    void SkyDriveDelete(QString, QStringList);
    void UbuntuOneDelete(QString, QStringList);

    QString DropboxCheckFile(QString Source, QStringList AccessToken);
    QString GoogleDriveCheckFile(QString fileID, QStringList AccessToken);
    QString UbuntuOneCheckFile(QString Source, QStringList AccessToken);
    QString SkyDriveCheckFile(QString fileID, QStringList AccessToken);

    QString GoogleDriveCreateBackup(QStringList AccessToken);
    QString SkyDriveCreateBackup(QStringList AccessToken);
    QString UbuntuOneCreateBackup(QStringList AccessToken);

private slots:
    void replyfinished1();
    void replyfinished2();
    void replyfinished3();
    void replyfinished3a();
    void replyfinished4();
    void replyfinished4a();
    void replyfinished5();
    void toSkyDriveUpload();
    void replyfinished6();
    void replyfinished6a();

    void dreplyfinished1();
    void dreplyfinished2();
    void dreplyfinished3();
    void dreplyfinished3a();
    void dreplyfinished4();
    void dreplyfinished4a();
    void dreplyfinished5();
    void toSkyDriveDownload();
    void dreplyfinished6();

    void dereplyfinished1();
    void dereplyfinished2();
    void dereplyfinished3();
    void dereplyfinished4();
    void dereplyfinished5();
    void toSkyDriveDelete();
    void dereplyfinished6();

    void Completed();

public:
    NetworkControllerParallel();
    NetworkControllerParallel(QStringList key_secret, QStringList mfolder_id);
    ~NetworkControllerParallel();

    QStringList InfoList; // there are 7 lists, and each list has two entries;
    QByteArray *download_streams;
    bool TokenIsChanged;

    QString Dropbox;
    QString GoogleDrive;
    QString SkyDrive;
    QString Ubuntu;

    QStringList *startUploadSession(QStringList TokenNames, QStringList *TokenList);

    QStringList *startDownloadSession(QStringList providers, QStringList TokenNames,
                                      QStringList *TokenList);

    void upload(QString, int, int, unsigned char *,
                int, QStringList *);

    void download(QStringList, QStringList*);

    void drop(QStringList, QStringList*);

    QStringList* getTokenList();

    QStringList* getDecodingTokenList();

    QStringList check(QStringList ListOfId, QStringList *TokenList);

    QStringList createBackup(QStringList *TokenList);

signals:
    void checkCompleted();

    void completed();
};

#endif // NETWORKCONTROLLERPARALLEL_H
