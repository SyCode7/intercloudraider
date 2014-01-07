#include "sessiondropbox.h"

SessionDropbox::SessionDropbox()
{
    request_uri = "https://api.dropbox.com/1/oauth/request_token";
    authorize_uri = "https://www.dropbox.com/1/oauth/authorize";
    access_uri = "https://api.dropbox.com/1/oauth/access_token";

    QStringList key_secret;
    DatabaseControllerSetting dbcontr;
    key_secret = dbcontr.retrieveKeySecret();
    key = key_secret[0];
    secret = key_secret[1];

    manager = new QNetworkAccessManager(this);
}

SessionDropbox::~SessionDropbox()
{
    delete manager;
}

QStringList SessionDropbox::getRequestToken()
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

void SessionDropbox::getAuthorization(QString RequestyKey)
{
    authorize_url = authorize_uri;
    authorize_url.addQueryItem("oauth_token", RequestyKey);

    QMessageBox msgBox;
    msgBox.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    msgBox.setWindowTitle("Dropbox Authorization");
    msgBox.setText("Please visit the following link and click 'Allow':");
    msgBox.setInformativeText(authorize_uri.append("?oauth_token=")
                              .append(RequestyKey));

    QPushButton openlinkbutton("Open Link");
    msgBox.addButton(&openlinkbutton, QMessageBox::YesRole);
    connect(&openlinkbutton, SIGNAL(clicked()), this, SLOT(openWebsite()));
    msgBox.exec();

    QMessageBox msgBox2;
    msgBox2.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    msgBox2.setWindowTitle("Proceed to next step");
    msgBox2.setText("After clicking 'Allow' at the pop-out website, "
                   "please click 'OK' to continue...");
    QPushButton *okbutton = msgBox2.addButton(tr("OK"), QMessageBox::YesRole);
    msgBox2.exec();

    if(msgBox2.clickedButton() == okbutton)
    {
        reply = manager->get(QNetworkRequest(authorize_url));

        // use an event loop to control the signal:
        QEventLoop loop;
        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();

        if(reply->error())
        {
            reply->close();
            params.clear();
        }
        else
        {
            reply->close();
            params.clear();
        }
    }
}

void SessionDropbox::openWebsite()
{
    QDesktopServices::openUrl(authorize_url);
}

QStringList SessionDropbox::getAccessToken(QStringList RequestToken)
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

        reply->close();
        params.clear();

        return List;
    }
    else
    {
        QString StringAccess_token_pair = reply->readAll();
        QStringList ListAccess_token_pair,
                    access_token_key,
                    access_token_secret,
                    access_token_uid;

        if(StringAccess_token_pair.contains("&") == true)
        {
            ListAccess_token_pair = StringAccess_token_pair.split("&");

            if(ListAccess_token_pair[0].contains("oauth_token_secret") == true)
            {
                access_token_secret = ListAccess_token_pair[0].split("=");
            }
            else if(ListAccess_token_pair[0].contains("oauth_token_secret") != true)
            {
                if(ListAccess_token_pair[0].contains("uid") == true)
                {
                    access_token_uid = ListAccess_token_pair[0].split("=");
                }
                else
                {
                    access_token_key = ListAccess_token_pair[0].split("=");
                }

            }

            if(ListAccess_token_pair[1].contains("oauth_token_secret") == true)
            {
                access_token_secret = ListAccess_token_pair[1].split("=");
            }
            else if(ListAccess_token_pair[1].contains("oauth_token_secret") != true)
            {
                if(ListAccess_token_pair[1].contains("uid") == true)
                {
                    access_token_uid = ListAccess_token_pair[1].split("=");
                }
                else
                {
                    access_token_key = ListAccess_token_pair[1].split("=");
                }
            }

            if(ListAccess_token_pair[2].contains("oauth_token_secret") == true)
            {
                access_token_secret = ListAccess_token_pair[1].split("=");
            }
            else if(ListAccess_token_pair[2].contains("oauth_token_secret") != true)
            {
                if(ListAccess_token_pair[2].contains("uid") == true)
                {
                    access_token_uid = ListAccess_token_pair[2].split("=");
                }
                else
                {
                    access_token_key = ListAccess_token_pair[2].split("=");
                }
            }
        }

        reply->close();
        params.clear();

        List.append(access_token_key[1]);
        List.append(access_token_secret[1]);
        List.append(access_token_uid[1]);

        return List;
    }
}
