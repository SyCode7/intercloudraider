#ifndef SESSIONDROPBOX_H
#define SESSIONDROPBOX_H

#include <QtNetwork>

#include <QEventLoop>

#include <QDesktopServices>
#include <QMessageBox>
#include <QPushButton>

#include "DBContr/databasecontrollersetting.h"

class SessionDropbox : public QObject
{
    Q_OBJECT

    QString request_uri, authorize_uri, access_uri,
            key, secret;

    QUrl params, authorize_url;
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

private slots:
    void openWebsite();

public:
    SessionDropbox();
    ~SessionDropbox();

    QStringList getRequestToken();
    void getAuthorization(QString);
    QStringList getAccessToken(QStringList);
};

#endif // SESSIONDROPBOX_H
