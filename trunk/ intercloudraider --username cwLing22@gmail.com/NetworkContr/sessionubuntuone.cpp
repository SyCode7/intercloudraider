#include "sessionubuntuone.h"

SessionUbuntuOne::SessionUbuntuOne()
{
    request_uri = "https://one.ubuntu.com/oauth/request/";
    authorize_uri = "https://one.ubuntu.com/oauth/authorize/";
    access_uri = "https://one.ubuntu.com/oauth/access/";

    key = "ubuntuone";
    secret = "hammertime";

    manager = new QNetworkAccessManager(this);
}

SessionUbuntuOne::~SessionUbuntuOne()
{
    delete manager;
}

QStringList SessionUbuntuOne::getRequestToken()
{
    QStringList List;

    params = request_uri;
    params.addQueryItem("oauth_consumer_key", key);
    params.addQueryItem("oauth_signature_method", "PLAINTEXT");
    params.addQueryItem("oauth_signature", QString(secret).append("&"));
    params.addQueryItem("oauth_timestamp", QString("%1")
                        .arg(QDateTime::currentDateTime().toTime_t()));

    qsrand(QDateTime::currentMSecsSinceEpoch());
    QString nonce = "";
    for(int i = 0; i < 8; i++)
    {
        nonce.append(QString("%1").arg(qrand() % 10));
    }
    params.addQueryItem("oauth_nonce", nonce);
    params.addQueryItem("oauth_version", "1.0");
    params.addQueryItem("oauth_callback", "http://my-site/oauth-return");

    QNetworkRequest request(params);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    reply = manager->post(request, params.encodedQuery());

    // use an event loop to control the signal:
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if(reply->error())
    {
        qDebug() << reply->errorString();
        qDebug() << reply->readAll();

        reply->close();
        params.clear();

        return List;
    }
    else
    {
        QString StringRequest_token_pair = reply->readAll();
        QStringList ListRequest_token_pair,
                    request_token_key,
                    request_token_secret;

        if(StringRequest_token_pair.contains("&") == true)
        {
            ListRequest_token_pair = StringRequest_token_pair.split("&");

            if(ListRequest_token_pair[0].contains("oauth_token_secret") == true)
            {
                request_token_secret = ListRequest_token_pair[0].split("=");
            }
            else if(ListRequest_token_pair[0].contains("oauth_token_secret") != true)
            {
                request_token_key = ListRequest_token_pair[0].split("=");
            }

            if(ListRequest_token_pair[1].contains("oauth_token_secret") == true)
            {
                request_token_secret = ListRequest_token_pair[1].split("=");
            }
            else if(ListRequest_token_pair[1].contains("oauth_token_secret") != true)
            {
                request_token_key = ListRequest_token_pair[1].split("=");
            }
        }

        reply->close();
        params.clear();

        List.append(request_token_key[1]);
        List.append(request_token_secret[1]);

        return List;
    }
}

bool SessionUbuntuOne::getAuthorization(QString RequestyKey)
{
    authorize_url = authorize_uri;
    authorize_url.addQueryItem("oauth_token", RequestyKey);

    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    msgBox.setWindowTitle("Ubuntu One Authorization");
    msgBox.setText("Please visit the following link and click 'Allow':");
    msgBox.setInformativeText(authorize_uri.append("?oauth_token=")
                              .append(RequestyKey));

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

void SessionUbuntuOne::openWebsite()
{
    QDesktopServices::openUrl(authorize_url);
}

QStringList SessionUbuntuOne::getAccessToken(QStringList RequestToken)
{
    QStringList List;

    params = access_uri;
    params.addQueryItem("oauth_consumer_key", key);
    params.addQueryItem("oauth_token", RequestToken[0]);
    params.addQueryItem("oauth_signature_method", "PLAINTEXT");

    // consumer secret + request_token secret
    params.addQueryItem("oauth_signature", QString(secret).append("&")
                         .append(RequestToken[1]));

    params.addQueryItem("oauth_timestamp",
                        QString("%1").arg(QDateTime::currentDateTime().toTime_t()));

    qsrand(QDateTime::currentMSecsSinceEpoch());
    QString nonce = "";
    for(int i = 0; i < 8; i++)
    {
        nonce.append(QString("%1").arg(qrand() % 10));
    }
    params.addQueryItem("oauth_nonce", nonce);
    params.addQueryItem("oauth_version", "1.0");
    params.addQueryItem("oauth_verifier", verification_code);

    QNetworkRequest request(params);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    reply = manager->post(request, params.encodedQuery());

    // use an event loop to control the signal:
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if(reply->error())
    {
        qDebug() << reply->errorString();
        qDebug() << reply->readAll();

        reply->close();
        params.clear();

        return List;
    }
    else
    {
        QString StringAccess_token_pair = reply->readAll();
        QStringList ListAccess_token_pair,
                    access_token_key,
                    access_token_secret;

        if(StringAccess_token_pair.contains("&") == true)
        {
            ListAccess_token_pair = StringAccess_token_pair.split("&");

            if(ListAccess_token_pair[0].contains("oauth_token_secret") == true)
            {
                access_token_secret = ListAccess_token_pair[0].split("=");
            }
            else if(ListAccess_token_pair[0].contains("oauth_token_secret") != true)
            {
                access_token_key = ListAccess_token_pair[0].split("=");
            }

            if(ListAccess_token_pair[1].contains("oauth_token_secret") == true)
            {
                access_token_secret = ListAccess_token_pair[1].split("=");
            }
            else if(ListAccess_token_pair[1].contains("oauth_token_secret") != true)
            {
                access_token_key = ListAccess_token_pair[1].split("=");
            }
        }

        reply->close();
        params.clear();

        List.append(access_token_key[1]);
        List.append(access_token_secret[1]);

        return List;
    }
}
