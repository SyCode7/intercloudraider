#include "sessionskydrive.h"

SessionSkyDrive::SessionSkyDrive()
{
    auth_uri = "https://login.live.com/oauth20_authorize.srf";
    token_uri = "https://login.live.com/oauth20_token.srf";

    QStringList key_secret;
    DatabaseControllerSetting dbcontr;
    key_secret = dbcontr.retrieveKeySecret();
    client_id = key_secret[4];
    client_secret = key_secret[5];

    scope = "wl.skydrive_update";

    manager = new QNetworkAccessManager(this);
}

SessionSkyDrive::~SessionSkyDrive()
{
    delete manager;
}

bool SessionSkyDrive::getAuthorization()
{
    authorize_url = auth_uri;
    authorize_url.addQueryItem("response_type", "code");
    authorize_url.addQueryItem("scope", scope);
    // authorize_url.addQueryItem("redirect_uri", "urn:ietf:wg:oauth:2.0:oob");
    authorize_url.addQueryItem("client_id", client_id);

    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    msgBox.setWindowTitle("SkyDrive Authorization");
    msgBox.setText("Please visit the following link and click 'Allow':");
    msgBox.setInformativeText(auth_uri.append("?").append("response_type=code&")
                              .append("scope=").append(scope).append("&\n")
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

void SessionSkyDrive::openWebsite()
{
    QDesktopServices::openUrl(authorize_url);
}

QStringList SessionSkyDrive::getAccessToken(QString code)
{
    QStringList List;

    params = token_uri;
    params.addQueryItem("grant_type", "authorization_code");
    params.addQueryItem("code", code);
    params.addQueryItem("client_id", client_id);
    params.addQueryItem("client_secret", client_secret);
    // params.addQueryItem("redirect_uri", "urn:ietf:wg:oauth:2.0:oob");

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

QStringList SessionSkyDrive::getNewAccessToken(QStringList RefreshToken)
{
    QStringList List;

    params = token_uri;
    params.addQueryItem("grant_type", "refresh_token");
    params.addQueryItem("refresh_token", RefreshToken[3]);
    params.addQueryItem("client_id", client_id);
    params.addQueryItem("client_secret", client_secret);
    // params.addQueryItem("redirect_uri", "https://login.live.com/oauth20_desktop.srf");

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
