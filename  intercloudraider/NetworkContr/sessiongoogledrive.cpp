#include "sessiongoogledrive.h"

SessionGoogleDrive::SessionGoogleDrive()
{
    auth_uri = "https://accounts.google.com/o/oauth2/auth";
    token_uri = "https://accounts.google.com/o/oauth2/token";

    QStringList key_secret;
    DatabaseControllerSetting dbcontr;
    key_secret = dbcontr.retrieveKeySecret();
    client_id = key_secret[2];
    client_secret = key_secret[3];

    manager = new QNetworkAccessManager(this);
}

SessionGoogleDrive::~SessionGoogleDrive()
{
    delete manager;
}

bool SessionGoogleDrive::getAuthorization()
{
    authorize_url = auth_uri;
    authorize_url.addQueryItem("response_type", "code");
    authorize_url.addQueryItem("scope", "https://www.googleapis.com/auth/drive");
    authorize_url.addQueryItem("redirect_uri", "urn:ietf:wg:oauth:2.0:oob");
    authorize_url.addQueryItem("client_id", client_id);

    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    msgBox.setWindowTitle("Google Drive Authorization");
    msgBox.setText("Please visit the following link and click 'Allow':");
    msgBox.setInformativeText(auth_uri.append("?").append("response_type=code&")
                              .append("scope=https://www.googleapis.com/auth/drive&\n")
                              .append("redirect_uri=urn:ietf:wg:oauth:2.0:oob&")
                              .append("client_id=").append(client_id));

    QPushButton openlinkbutton("Open Link");
    msgBox.addButton(&openlinkbutton, QMessageBox::YesRole);
    connect(&openlinkbutton, SIGNAL(clicked()), this, SLOT(openWebsite()));
    msgBox.exec();

    GetCodeDialog cdialog;
    cdialog.exec();

    verification_code = cdialog.code;

    if(!verification_code.isEmpty())
        return true;
    else
        return false;
}

void SessionGoogleDrive::openWebsite()
{
    QDesktopServices::openUrl(authorize_url);
}

QStringList SessionGoogleDrive::getAccessToken(QString code)
{
    QStringList List;

    params = token_uri;
    params.addQueryItem("grant_type", "authorization_code");
    params.addQueryItem("code", code);
    params.addQueryItem("client_id", client_id);
    params.addQueryItem("client_secret", client_secret);
    params.addQueryItem("redirect_uri", "urn:ietf:wg:oauth:2.0:oob");

    QNetworkRequest request(params);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");

    reply = manager->post(request, params.encodedQuery());

    // use an event loop to control the signal:
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if(reply->error())
    {
        qDebug() << reply->errorString();

        reply->close();
        params.clear();

        return List;
    }
    else
    {
        QString access_token,
                token_type,
                refresh_token;
        int expires_in;
        QByteArray json;
        json = reply->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");
        }
        else
        {
            access_token = result["access_token"].toString();
            token_type = result["token_type"].toString();
            expires_in = result["expires_in"].toUInt();
            refresh_token = result["refresh_token"].toString();
        }

        reply->close();
        params.clear();

        List.append(access_token);
        List.append(token_type);
        List.append((QString::number(expires_in)).append("000"));
        List.append(refresh_token);

        return List;
    }
}

QStringList SessionGoogleDrive::getNewAccessToken(QStringList RefreshToken)
{
    QStringList List;

    params = token_uri;
    params.addQueryItem("grant_type", "refresh_token");
    params.addQueryItem("refresh_token", RefreshToken[3]);
    params.addQueryItem("client_id", client_id);
    params.addQueryItem("client_secret", client_secret);

    QNetworkRequest request(params);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");

    reply = manager->post(request, params.encodedQuery());

    // use an event loop to control the signal:
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if(reply->error())
    {
        qDebug() << reply->errorString();

        reply->close();
        params.clear();

        return List;
    }
    else
    {
        QString access_token,
                token_type;
        int expires_in;
        QByteArray json;
        json = reply->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");
        }
        else
        {
            access_token = result["access_token"].toString();
            token_type = result["token_type"].toString();
            expires_in = result["expires_in"].toUInt();
        }

        reply->close();
        params.clear();

        List.append(access_token);
        List.append(token_type);
        List.append((QString::number(expires_in)).append("000"));
        List.append(RefreshToken[3]);

        return List;
    }
}
