#ifndef SESSIONUBUNTUONE_H
#define SESSIONUBUNTUONE_H

#include <QtNetwork>

#include <QEventLoop>

#include <QDesktopServices>
#include <QMessageBox>
#include <QPushButton>

#include "GUI/getcodedialog.h"

class SessionUbuntuOne : public QObject
{
    Q_OBJECT

    QString request_uri, authorize_uri, access_uri,
            key, secret, verification_code;

    QUrl params, authorize_url;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

private slots:
    void openWebsite();

public:
    SessionUbuntuOne();
    ~SessionUbuntuOne();

    QStringList getRequestToken();
    bool getAuthorization(QString);
    QStringList getAccessToken(QStringList);
};

#endif // SESSIONUBUNTUONE_H
