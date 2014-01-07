#ifndef SESSIONSKYDRIVE_H
#define SESSIONSKYDRIVE_H

#include <QtNetwork>

#include <QEventLoop>

#include <QDesktopServices>
#include <QMessageBox>
#include <QPushButton>

// QJson
#include "parser.h"
#include "GUI/getcodedialog.h"
#include "DBContr/databasecontrollersetting.h"

class SessionSkyDrive : public QObject
{
    Q_OBJECT

    QString auth_uri, token_uri, scope,
            client_id, client_secret,
            verification_code;

    QUrl params, authorize_url;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

private slots:
    void openWebsite();

public:
    SessionSkyDrive();
    ~SessionSkyDrive();

    bool getAuthorization();
    QStringList getAccessToken(QString);
    QString code()
    {
        return verification_code;
    }

    QStringList getNewAccessToken(QStringList);
};

#endif // SESSIONSKYDRIVE_H
