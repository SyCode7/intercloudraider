#ifndef SESSIONGOOGLEDRIVE_H
#define SESSIONGOOGLEDRIVE_H

#include <QtNetwork>

#include <QEventLoop>

#include <QDesktopServices>
#include <QMessageBox>
#include <QPushButton>

// QJson
#include "parser.h"
#include "GUI/getcodedialog.h"
#include "DBContr/databasecontrollersetting.h"

class SessionGoogleDrive : public QObject
{
    Q_OBJECT

    QString auth_uri, token_uri,
            client_id, client_secret,
            verification_code;

    QUrl params, authorize_url;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

private slots:
    void openWebsite();

public:
    SessionGoogleDrive();
    ~SessionGoogleDrive();

    bool getAuthorization();
    QStringList getAccessToken(QString);
    QString code()
    {
        return verification_code;
    }

    QStringList getNewAccessToken(QStringList);

};

#endif // SESSIONGOOGLEDRIVE_H
