#include "networkcontrollerparallel.h"

// for creating backup folder:
NetworkControllerParallel::NetworkControllerParallel()
{
    GoogleDrive_folder_uri = "https://www.googleapis.com/drive/v2/files";
    SkyDrive_folder_uri = "https://apis.live.net/v5.0/me/skydrive";

    Ubuntu_key = "ubuntuone";
    Ubuntu_secret = "hammertime";
    base_uri = "https://one.ubuntu.com";
    file_uri = "https://files.one.ubuntu.com";
    api = "/api/file_storage/v1/";
    volume = "~/Ubuntu%20One/";

    manager = new QNetworkAccessManager;
}

// for network operation:
NetworkControllerParallel::NetworkControllerParallel(QStringList key_secret,
                                                     QStringList mfolder_id)
{
    folder_id = mfolder_id;

    Dropbox_key = key_secret[0];
    Dropbox_secret = key_secret[1];

    Dropbox_upload_uri = "https://api-content.dropbox.com/1/files_put/sandbox/";
    Dropbox_download_uri = "https://api-content.dropbox.com/1/files/sandbox/";
    Dropbox_delete_uri = "https://api.dropbox.com/1/fileops/delete";
    Dropbox_metadata_uri = "https://api.dropbox.com/1/metadata/sandbox/";

    GoogleDrive_upload_uri = "https://www.googleapis.com/upload/drive/v2/files";
    GoogleDrive_download_uri = "https://www.googleapis.com/drive/v2/files/";
    GoogleDrive_folder_uri = "https://www.googleapis.com/drive/v2/files";

    Ubuntu_key = "ubuntuone";
    Ubuntu_secret = "hammertime";
    base_uri = "https://one.ubuntu.com";
    file_uri = "https://files.one.ubuntu.com";
    api = "/api/file_storage/v1/";
    volume = "~/Ubuntu%20One/";

    SkyDrive_upload_uri = QString("https://apis.live.net/v5.0/%1/files/")
            .arg(folder_id[1]);
    SkyDrive_download_uri = "https://apis.live.net/v5.0/";
    SkyDrive_folder_uri = "https://apis.live.net/v5.0/me/skydrive";

    ctr = 0;
    dctr = 0;
    dectr = 0;

    manager = new QNetworkAccessManager;

    TokenIsChanged = false;

    connect(this, SIGNAL(checkCompleted()), this, SLOT(Completed()));
}

NetworkControllerParallel::~NetworkControllerParallel()
{
}

QStringList *NetworkControllerParallel::startUploadSession(QStringList TokenNames,
                                                           QStringList *TokenList)
{
    QStringList *EncodingTokenList;
    EncodingTokenList = new QStringList[4];

    if(TokenNames.contains("Dropbox"))
    {
        EncodingTokenList[0] = TokenList[0];
    }
    else
    {
        // Dropbox:
        QStringList DropboxRequestToken, DropboxAccessToken;

        SessionDropbox DropboxSession;
        DropboxRequestToken = DropboxSession.getRequestToken();
        DropboxSession.getAuthorization(DropboxRequestToken[0]);
        DropboxAccessToken = DropboxSession.getAccessToken(DropboxRequestToken);

        if(!DropboxAccessToken.isEmpty())
        {
            EncodingTokenList[0] = DropboxAccessToken;
            Dropbox = "Dropbox";
        }
        else
        {
            return startUploadSession(TokenNames, TokenList);
        }
    }

    if(TokenNames.contains("Google Drive"))
    {
        EncodingTokenList[1] = TokenList[1];
    }
    else
    {
        // Google Drive:
        QStringList GoogleDriveAccessToken;

        SessionGoogleDrive GoogleDriveSession;
        if(GoogleDriveSession.getAuthorization())
        {
            GoogleDriveAccessToken = GoogleDriveSession
                    .getAccessToken(GoogleDriveSession.code());

            EncodingTokenList[1] = GoogleDriveAccessToken;
            GoogleDrive = "Google Drive";
        }
        else
        {
            return startUploadSession(TokenNames, TokenList);
        }
    }

    if(TokenNames.contains("SkyDrive"))
    {
        EncodingTokenList[2] = TokenList[2];
    }
    else
    {
        // SkyDrive:
        QStringList SkyDriveAccessToken;

        SessionSkyDrive SkyDriveSession;
        if(SkyDriveSession.getAuthorization())
        {
            SkyDriveAccessToken = SkyDriveSession
                    .getAccessToken(SkyDriveSession.code());

            EncodingTokenList[2] = SkyDriveAccessToken;
            SkyDrive = "SkyDrive";
        }
        else
        {
            return startUploadSession(TokenNames, TokenList);
        }
    }

    if(TokenNames.contains("Ubuntu"))
    {
        EncodingTokenList[3] = TokenList[3];
    }
    else
    {
        // UbuntuOne:
        QStringList UbuntuRequestToken, UbuntuAccessToken;

        SessionUbuntuOne UbuntuSession;
        UbuntuRequestToken = UbuntuSession.getRequestToken();
        if(UbuntuSession.getAuthorization(UbuntuRequestToken[0]))
        {
            UbuntuAccessToken = UbuntuSession
                    .getAccessToken(UbuntuRequestToken);

            EncodingTokenList[3] = UbuntuAccessToken;
            Ubuntu = "Ubuntu";
        }
        else
        {
            return startUploadSession(TokenNames, TokenList);
        }
    }

    return EncodingTokenList;
}

void NetworkControllerParallel::upload(QString md5, int n, int k, unsigned char *encoded_vector,
                                       int padded_size, QStringList *TokenList)
{
    InfoList << "" << "" << "" << "" << "" << "" << "";

    mTokenList = new QStringList[4];
    mTokenList = TokenList;

    N = n;
    mK = k;
    size = padded_size;
    encoded_parts = new unsigned char*[n];

    for(int i = 0; i < n; i++)
        encoded_parts[i] = new unsigned char[size/k];

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < (size/k); j++)
            encoded_parts[i][j] = encoded_vector[j*n + i];

        if(i < n - 1)
        {
            QString chunk_name = "";
            chunk_name.append(md5);
            chunk_name.append(QString(".encoded%1").arg(i + 1));
            chunk_name.append(".bin");

            // Mask the chunk_name by Md4.
            QString md4;
            EngineCryptographicHashing hashing;
            md4 = hashing.calculateHash(chunk_name);

            blockname.append(md4);
        }
        else
        {
            if(!QDir("local").exists())
            {
                QDir().mkpath("local");
            }

            // store encoded blocks as local copies:
            QString chunk_name = "";
            chunk_name.append(md5);
            chunk_name.append(QString(".encoded%1").arg(i + 1));
            chunk_name.append(".bin");

            // Mask the block_name by Md4:
            QString md4;
            EngineCryptographicHashing hashing;
            md4 = hashing.calculateHash(chunk_name);

            QFile file("./local/" + md4);
            if(!file.open(QIODevice::WriteOnly))
            {
                qDebug() << QString("Unable to open file!");
            }
            else
            {
                QByteArray upload_stream;
                for(int l = 0; l < size/k; l++)
                    upload_stream.append(encoded_parts[n - 1][l]);

                file.write(upload_stream);

                file.flush();
                file.close();

                InfoList[n - 1] = "./local/" + md4;
            }
        }
    }

    // do parallel uploadings:
    DropboxUpload(blockname[0], size/k, encoded_parts[0], TokenList[0]);
    DropboxUpload2(blockname[1], size/k, encoded_parts[1], TokenList[0]);
    GoogleDriveUpload(blockname[3], size/k, TokenList[1]);
    GoogleDriveUpload2(blockname[5], size/k, TokenList[1]);
    SkyDriveUpload(blockname[2], size/k, encoded_parts[2], TokenList[2]);
    UbuntuOneUpload(blockname[4], TokenList[3]);
}

QUrl NetworkControllerParallel::setDropboxUrl(QString Source, QString uri,
                                              QStringList AccessToken)
{
    QUrl url(uri + Source);
    url.addQueryItem("oauth_consumer_key", Dropbox_key);

    url.addQueryItem("oauth_token", AccessToken[0]);
    url.addQueryItem("oauth_signature_method", "PLAINTEXT");

    // consumer secret + access_token secret
    url.addQueryItem("oauth_signature", QString(Dropbox_secret).append("&")
                      .append(AccessToken[1]));

    url.addQueryItem("oauth_timestamp", QString("%1")
                     .arg(QDateTime::currentDateTime().toTime_t()));

    qsrand(QDateTime::currentMSecsSinceEpoch());
    QString nonce = "";
    for(int i = 0; i < 8; i++)
    {
        nonce.append(QString("%1").arg(qrand() % 10));
    }
    url.addQueryItem("oauth_nonce", nonce);
    url.addQueryItem("oauth_version", "1.0");
    url.addQueryItem("uid", AccessToken[2]);

    return url;
}

void NetworkControllerParallel::DropboxUpload(QString block_name, int size,
                                              unsigned char *encoded_parts,
                                              QStringList AccessToken)
{
    QByteArray upload_stream;

    for(int i = 0; i < size; i++)
        upload_stream.append(encoded_parts[i]);

    QUrl put_url = setDropboxUrl(block_name, Dropbox_upload_uri, AccessToken);

    QNetworkRequest request(put_url);
    reply1 = manager->put(request, QByteArray(upload_stream, size));
    connect(reply1, SIGNAL(finished()), this, SLOT(replyfinished1()));
}

void NetworkControllerParallel::replyfinished1()
{
    if(reply1->error())
    {
        qDebug() << reply1->errorString();
        qDebug() << reply1->readAll();

        DropboxUpload(blockname[0], size/mK, encoded_parts[0], mTokenList[0]);
    }
    else
    {
        QByteArray json;
        json = reply1->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            DropboxUpload(blockname[0], size/mK, encoded_parts[0], mTokenList[0]);
        }
        else
        {
            reply1->deleteLater();

            QString id = result["path"].toString();
            InfoList[0] = id;

            ctr++;

            emit checkCompleted();
        }
    }
}

void NetworkControllerParallel::DropboxUpload2(QString block_name, int size,
                                               unsigned char *encoded_parts,
                                               QStringList AccessToken)
{
    QByteArray upload_stream;

    for(int i = 0; i < size; i++)
        upload_stream.append(encoded_parts[i]);

    QUrl put_url = setDropboxUrl(block_name, Dropbox_upload_uri, AccessToken);

    QNetworkRequest request(put_url);
    reply2 = manager->put(request, QByteArray(upload_stream, size));
    connect(reply2, SIGNAL(finished()), this, SLOT(replyfinished2()));
}

void NetworkControllerParallel::replyfinished2()
{
    if(reply2->error())
    {
        qDebug() << reply2->errorString();
        qDebug() << reply2->readAll();

        DropboxUpload2(blockname[1], size/mK, encoded_parts[1], mTokenList[0]);
    }
    else
    {
        QByteArray json;
        json = reply2->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            DropboxUpload2(blockname[1], size/mK, encoded_parts[1], mTokenList[0]);
        }
        else
        {
            reply2->deleteLater();

            QString id = result["path"].toString();
            InfoList[1] = id;

            ctr++;

            emit checkCompleted();
        }
    }
}

QUrl NetworkControllerParallel::setGoogleUrl(QString uri, QStringList AccessToken)
{
    QUrl resumable_url(uri);
    resumable_url.addQueryItem("uploadType", "resumable");
    resumable_url.addQueryItem("access_token", AccessToken[0]);

    return resumable_url;
}

QByteArray NetworkControllerParallel::setGoogleMetadata(QString Source)
{
    QByteArray metadata("");
    metadata.append("{\n");
    metadata.append("  \"title\": \"").append(Source).append("\",\n");
    metadata.append("  \"parents\": [{\n");
    metadata.append("  \"kind\" : \"drive#file\",\n");
    metadata.append("  \"id\": \"").append(folder_id[0]).append("\"\n");
    metadata.append("}]\n");
    metadata.append("}");

    return metadata;
}

QNetworkRequest NetworkControllerParallel::setGoogleRequest(QUrl url, int metadata_size,
                                                            int size)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/json; charset=UTF-8");
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      QString::number(metadata_size));

    request.setRawHeader("X-Upload-Content-Type", "application/octet-stream");
    request.setRawHeader("X-Upload-Content-Length",
                         QString::number(size).toUtf8());

    return request;
}

void NetworkControllerParallel::GoogleDriveUpload(QString block_name, int size,
                                                  QStringList AccessToken)
{
    QUrl resumable_url = setGoogleUrl(GoogleDrive_upload_uri, AccessToken);
    QByteArray metadata = setGoogleMetadata(block_name);
    QNetworkRequest request = setGoogleRequest(resumable_url, metadata.size(), size);

    reply3 = manager->post(request, metadata);
    connect(reply3, SIGNAL(finished()), this, SLOT(replyfinished3()));
}

void NetworkControllerParallel::replyfinished3()
{
    if(reply3->error())
    {
        qDebug() << reply3->errorString();

        QByteArray json;
        json = reply3->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            GoogleDriveUpload(blockname[3], size/mK, mTokenList[1]);
        }
        else
        {
            int code;
            QVariantMap error = result["error"].toMap();
            code = error["code"].toInt();
            qDebug() << error["message"].toString();

            if(code == 401)
            {
                SessionGoogleDrive session;
                mTokenList[1] = session.getNewAccessToken(mTokenList[1]);
                TokenIsChanged = true;

                GoogleDriveUpload(blockname[3], size/mK, mTokenList[1]);
            }
            else
            {
                GoogleDriveUpload(blockname[3], size/mK, mTokenList[1]);
            }
        }
    }
    else
    {
        QString uploadUrl = reply3->rawHeader("Location");
        QUrl upload_url(uploadUrl);
        QNetworkRequest request(upload_url);

        QByteArray upload_stream;

        for(int i = 0; i < size/mK; i++)
            upload_stream.append(encoded_parts[3][i]);

        reply3 = manager->put(request, QByteArray(upload_stream, size/mK));
        connect(reply3, SIGNAL(finished()), this, SLOT(replyfinished3a()));
    }
}

void NetworkControllerParallel::replyfinished3a()
{
    if(reply3->error())
    {
        qDebug() << reply3->errorString();
        qDebug() << reply3->readAll();

        GoogleDriveUpload(blockname[3], size/mK, mTokenList[1]);
    }
    else
    {
        // qDebug() << reply->rawHeaderPairs();
        // qDebug() << reply->readAll();

        QByteArray json;
        json = reply3->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            GoogleDriveUpload(blockname[3], size/mK, mTokenList[1]);
        }
        else
        {
            reply3->deleteLater();

            QString id = result["id"].toString();
            InfoList[3] = id;

            ctr++;

            emit checkCompleted();
        }
    }
}

void NetworkControllerParallel::GoogleDriveUpload2(QString block_name, int size,
                                                  QStringList AccessToken)
{
    QUrl resumable_url = setGoogleUrl(GoogleDrive_upload_uri, AccessToken);
    QByteArray metadata = setGoogleMetadata(block_name);
    QNetworkRequest request = setGoogleRequest(resumable_url, metadata.size(), size);
    reply4 = manager->post(request, metadata);
    connect(reply4, SIGNAL(finished()), this, SLOT(replyfinished4()));
}


void NetworkControllerParallel::replyfinished4()
{
    if(reply4->error())
    {
        qDebug() << reply4->errorString();

        QByteArray json;
        json = reply4->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            GoogleDriveUpload2(blockname[5], size/mK, mTokenList[1]);
        }
        else
        {
            int code;
            QVariantMap error = result["error"].toMap();
            code = error["code"].toInt();
            qDebug() << error["message"].toString();

            if(code == 401)
            {
                SessionGoogleDrive session;
                mTokenList[1] = session.getNewAccessToken(mTokenList[1]);
                TokenIsChanged = true;

                GoogleDriveUpload2(blockname[5], size/mK, mTokenList[1]);
            }
            else
            {
                GoogleDriveUpload2(blockname[5], size/mK, mTokenList[1]);
            }
        }
    }
    else
    {
        QString uploadUrl = reply4->rawHeader("Location");
        QUrl upload_url(uploadUrl);
        QNetworkRequest request(upload_url);

        QByteArray upload_stream;

        for(int i = 0; i < size/mK; i++)
            upload_stream.append(encoded_parts[5][i]);

        reply4 = manager->put(request, QByteArray(upload_stream, size/mK));
        connect(reply4, SIGNAL(finished()), this, SLOT(replyfinished4a()));
    }
}

void NetworkControllerParallel::replyfinished4a()
{
    if(reply4->error())
    {
        qDebug() << reply4->errorString();
        qDebug() << reply4->readAll();

        GoogleDriveUpload2(blockname[5], size/mK, mTokenList[1]);
    }
    else
    {
        // qDebug() << reply->rawHeaderPairs();
        // qDebug() << reply->readAll();

        QByteArray json;
        json = reply4->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            GoogleDriveUpload2(blockname[5], size/mK, mTokenList[1]);
        }
        else
        {
            reply4->deleteLater();

            QString id = result["id"].toString();
            InfoList[5] = id;

            ctr++;

            emit checkCompleted();
        }
    }
}

void NetworkControllerParallel::SkyDriveUpload(QString block_name, int size,
                                               unsigned char *encoded_parts,
                                               QStringList AccessToken)
{
    QByteArray upload_stream;

    for(int i = 0; i < size; i++)
        upload_stream.append(encoded_parts[i]);

    QUrl put_url(SkyDrive_upload_uri + block_name);
    put_url.addQueryItem("access_token", AccessToken[0]);

    QNetworkRequest request(put_url);

    reply5 = manager->put(request, QByteArray(upload_stream, size));
    connect(reply5, SIGNAL(finished()), this, SLOT(replyfinished5()));
}

void NetworkControllerParallel::replyfinished5()
{
    if(reply5->error())
    {
        qDebug() << reply5->errorString();

        QByteArray json;
        json = reply5->readAll();
        qDebug() << json;

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            SkyDriveUpload(blockname[2], size/mK, encoded_parts[2], mTokenList[2]);
        }
        else
        {
            QString code;
            QVariantMap error = result["error"].toMap();
            code = error["code"].toString();
            qDebug() << error["message"].toString();

            if(code.contains("request_token_expired"))
            {
                SessionSkyDrive session;
                mTokenList[2] = session.getNewAccessToken(mTokenList[2]);
                TokenIsChanged = true;

                SkyDriveUpload(blockname[2], size/mK, encoded_parts[2], mTokenList[2]);
            }
            else if(code.contains("request_throttled"))
            {
                QTimer::singleShot(600000, this, SLOT(toSkyDriveUpload()));
            }
            else
            {
                SkyDriveUpload(blockname[2], size/mK, encoded_parts[2], mTokenList[2]);
            }
        }
    }
    else
    {
        // qDebug() << reply->rawHeaderPairs();
        // qDebug() << reply->readAll();

        QByteArray json;
        json = reply5->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            SkyDriveUpload(blockname[2], size/mK, encoded_parts[2], mTokenList[2]);
        }
        else
        {
            reply5->deleteLater();

            QString id = result["id"].toString();
            InfoList[2] = id;

            ctr++;

            emit checkCompleted();
        }
    }
}

void NetworkControllerParallel::toSkyDriveUpload()
{
    SkyDriveUpload(blockname[2], size/mK, encoded_parts[2], mTokenList[2]);
}

QUrl NetworkControllerParallel::setUbuntuUrl(QString Source, QString uri,
                                             QStringList AccessToken)
{
    QUrl url(uri + Source);
    url.addQueryItem("oauth_consumer_key", Ubuntu_key);
    url.addQueryItem("oauth_token", AccessToken[0]);
    url.addQueryItem("oauth_signature_method", "PLAINTEXT");

    // consumer secret + access_token secret
    url.addQueryItem("oauth_signature", QString(Ubuntu_secret).append("&")
                         .append(AccessToken[1]));

    url.addQueryItem("oauth_timestamp",
                        QString("%1").arg(QDateTime::currentDateTime().toTime_t()));

    qsrand(QDateTime::currentMSecsSinceEpoch());
    QString nonce = "";
    for(int i = 0; i < 8; i++)
    {
        nonce.append(QString("%1").arg(qrand() % 10));
    }
    url.addQueryItem("oauth_nonce", nonce);
    url.addQueryItem("oauth_version", "1.0");

    return url;
}

void NetworkControllerParallel::UbuntuOneUpload(QString block_name,
                                                QStringList AccessToken)
{
    QUrl put_url = setUbuntuUrl(block_name, base_uri + api + volume + "HSRC_backup/",
                                AccessToken);

    QByteArray metadata("");
    metadata.append("{\n");
    metadata.append("  \"kind\": \"").append("file").append("\"\n");
    metadata.append("}");

    QNetworkRequest request(put_url);

    reply6 = manager->put(request, metadata);
    connect(reply6, SIGNAL(finished()), this, SLOT(replyfinished6()));
}

void NetworkControllerParallel::replyfinished6()
{
    if(reply6->error())
    {
        qDebug() << reply6->errorString();
        qDebug() << reply6->readAll();

        UbuntuOneUpload(blockname[4], mTokenList[3]);
    }
    else
    {
        QByteArray json;
        json = reply6->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            UbuntuOneUpload(blockname[4], mTokenList[3]);
        }
        else
        {
            QByteArray upload_stream;
            QStringList AccessToken = mTokenList[3];

            for(int i = 0; i < size/mK; i++)
                upload_stream.append(encoded_parts[4][i]);

            QString content_path = result["content_path"].toString();
            QUrl upload_url = setUbuntuUrl(content_path, file_uri, AccessToken);

            QNetworkRequest request(upload_url);
            request.setHeader(QNetworkRequest::ContentTypeHeader,
                              "application/octet-stream");
            request.setHeader(QNetworkRequest::ContentLengthHeader,
                              QString::number(size/mK));

            reply6 = manager->put(request, QByteArray(upload_stream, size/mK));
            connect(reply6, SIGNAL(finished()), this, SLOT(replyfinished6a()));
        }
    }
}

void NetworkControllerParallel::replyfinished6a()
{
    if(reply6->error())
    {
        qDebug() << reply6->errorString();
        qDebug() << reply6->readAll();

        UbuntuOneUpload(blockname[4], mTokenList[3]);
    }
    else
    {
        QByteArray json;
        json = reply6->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            UbuntuOneUpload(blockname[4], mTokenList[3]);
        }
        else
        {
            reply6->deleteLater();

            QString id = result["content_path"].toString();
            InfoList[4] = id;

            ctr++;

            emit checkCompleted();
        }
    }
}

QStringList *NetworkControllerParallel::startDownloadSession(QStringList providers,
                                                             QStringList TokenNames,
                                                             QStringList *TokenList)
{
    QStringList *DecodingTokenList;
    DecodingTokenList = new QStringList[2];

    if(providers.contains("Dropbox"))
    {
        if(TokenNames.contains("Dropbox"))
        {
            DecodingTokenList[0] = TokenList[0];
            DecodingTokenList[0].append("Dropbox");
            return DecodingTokenList;
        }
        else
        {
            // Dropbox:
            QStringList DropboxRequestToken, DropboxAccessToken;

            SessionDropbox DropboxSession;
            DropboxRequestToken = DropboxSession.getRequestToken();
            DropboxSession.getAuthorization(DropboxRequestToken[0]);
            DropboxAccessToken = DropboxSession.getAccessToken(DropboxRequestToken);

            if(!DropboxAccessToken.isEmpty())
            {
                DecodingTokenList[0] = DropboxAccessToken;
                DecodingTokenList[0].append("Dropbox");
                Dropbox = "Dropbox";
                return DecodingTokenList;
            }
            else
            {
                return startDownloadSession(providers, TokenNames, TokenList);
            }
        }
    }

    if(providers.contains("Google Drive"))
    {
        if(TokenNames.contains("Google Drive"))
        {
            DecodingTokenList[0] = TokenList[1];
            DecodingTokenList[0].append("Google Drive");
            return DecodingTokenList;
        }
        else
        {
            // Google Drive:
            QStringList GoogleDriveAccessToken;

            SessionGoogleDrive GoogleDriveSession;
            if(GoogleDriveSession.getAuthorization())
            {
                GoogleDriveAccessToken = GoogleDriveSession
                        .getAccessToken(GoogleDriveSession.code());

                DecodingTokenList[0] = GoogleDriveAccessToken;
                DecodingTokenList[0].append("Google Drive");
                GoogleDrive = "Google Drive";
                return DecodingTokenList;
            }
            else
            {
                return startDownloadSession(providers, TokenNames, TokenList);
            }
        }
    }

    if(providers.contains("SkyDrive") && providers.contains("Ubuntu"))
    {
        if(TokenNames.contains("SkyDrive"))
        {
            DecodingTokenList[0] = TokenList[2];
            DecodingTokenList[0].append("SkyDrive");
        }
        else
        {
            // SkyDrive:
            QStringList SkyDriveAccessToken;

            SessionSkyDrive SkyDriveSession;
            if(SkyDriveSession.getAuthorization())
            {
                SkyDriveAccessToken = SkyDriveSession
                        .getAccessToken(SkyDriveSession.code());

                DecodingTokenList[0] = SkyDriveAccessToken;
                DecodingTokenList[0].append("SkyDrive");
                SkyDrive = "SkyDrive";
            }
            else
            {
                return startDownloadSession(providers, TokenNames, TokenList);
            }
        }

        if(TokenNames.contains("Ubuntu"))
        {
            DecodingTokenList[1] = TokenList[3];
            DecodingTokenList[1].append("Ubuntu");
        }
        else
        {
            // UbuntuOne:
            QStringList UbuntuRequestToken, UbuntuAccessToken;

            SessionUbuntuOne UbuntuSession;
            UbuntuRequestToken = UbuntuSession.getRequestToken();
            if(UbuntuSession.getAuthorization(UbuntuRequestToken[0]))
            {
                UbuntuAccessToken = UbuntuSession
                        .getAccessToken(UbuntuRequestToken);

                DecodingTokenList[1] = UbuntuAccessToken;
                DecodingTokenList[1].append("Ubuntu");
                Ubuntu = "Ubuntu";
            }
            else
            {
                return startDownloadSession(providers, TokenNames, TokenList);
            }
        }

        return DecodingTokenList;
    }
}

void NetworkControllerParallel::download(QStringList ListOfId, QStringList *TokenList)
{
    download_streams = new QByteArray[2];
    mListOfId = ListOfId;
    dTokenList = TokenList;

    if(TokenList[0].contains("Dropbox"))
    {
        // do parallel downloadings:
        DropboxDownload(ListOfId[0], TokenList[0]);
        DropboxDownload2(ListOfId[1], TokenList[0]);
    }

    else if(TokenList[0].contains("Google Drive"))
    {
        // do parallel downloadings:
        GoogleDriveDownload(ListOfId[3], TokenList[0]);
        GoogleDriveDownload2(ListOfId[5], TokenList[0]);
    }

    else if(TokenList[0].contains("SkyDrive") && TokenList[1].contains("Ubuntu"))
    {
        // do parallel downloadings:
        SkyDriveDownload(ListOfId[2], TokenList[0]);
        UbuntuOneDownload(ListOfId[4], TokenList[1]);
    }
}

void NetworkControllerParallel::DropboxDownload(QString Source, QStringList AccessToken)
{
    QUrl get_url = setDropboxUrl(Source, Dropbox_download_uri, AccessToken);

    QNetworkRequest request(get_url);
    reply1 = manager->get(request);
    connect(reply1, SIGNAL(finished()), this, SLOT(dreplyfinished1()));
}

void NetworkControllerParallel::dreplyfinished1()
{
    if(reply1->error())
    {
        qDebug() << reply1->errorString();

        QByteArray json;
        json = reply1->readAll();
        qDebug() << json;

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            DropboxDownload(mListOfId[0], dTokenList[0]);
        }
        else
        {
            QString error = result["error"].toString();

            if(error.contains("deleted"))
            {
                qDebug() << error;
                download_streams[0] = "";

                // retrieve coordinates:
                (download_streams[0]).append(QByteArray::number(0));

                reply1->deleteLater();

                dctr++;

                emit checkCompleted();
            }
            else
            {
                DropboxDownload(mListOfId[0], dTokenList[0]);
            }
        }
    }
    else
    {
        // qDebug() << reply->rawHeaderPairs();

        download_streams[0] = reply1->readAll();

        // retrieve coordinates:
        (download_streams[0]).append(QByteArray::number(0));

        reply1->deleteLater();

        dctr++;

        emit checkCompleted();
    }
}

void NetworkControllerParallel::DropboxDownload2(QString Source, QStringList AccessToken)
{
    QUrl get_url = setDropboxUrl(Source, Dropbox_download_uri, AccessToken);

    QNetworkRequest request(get_url);
    reply2 = manager->get(request);
    connect(reply2, SIGNAL(finished()), this, SLOT(dreplyfinished2()));
}

void NetworkControllerParallel::dreplyfinished2()
{
    if(reply2->error())
    {
        qDebug() << reply2->errorString();

        QByteArray json;
        json = reply2->readAll();
        qDebug() << json;

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            DropboxDownload2(mListOfId[1], dTokenList[0]);
        }
        else
        {
            QString error = result["error"].toString();

            if(error.contains("deleted"))
            {
                qDebug() << error;
                download_streams[1] = "";

                // retrieve coordinates:
                (download_streams[1]).append(QByteArray::number(1));

                reply2->deleteLater();

                dctr++;

                emit checkCompleted();
            }
            else
            {
                DropboxDownload2(mListOfId[1], dTokenList[0]);
            }
        }
    }
    else
    {
        // qDebug() << reply->rawHeaderPairs();

        download_streams[1] = reply2->readAll();

        // retrieve coordinates:
        (download_streams[1]).append(QByteArray::number(1));

        reply2->deleteLater();

        dctr++;

        emit checkCompleted();
    }
}

void NetworkControllerParallel::GoogleDriveDownload(QString fileID, QStringList AccessToken)
{
    QUrl get_url(GoogleDrive_download_uri + fileID);
    get_url.addQueryItem("access_token", AccessToken[0]);

    QNetworkRequest request(get_url);
    reply3 = manager->get(request);
    connect(reply3, SIGNAL(finished()), this, SLOT(dreplyfinished3()));
}

void NetworkControllerParallel::dreplyfinished3()
{
    if(reply3->error())
    {
        qDebug() << reply3->errorString();

        QByteArray json;
        json = reply3->readAll();
        qDebug() << json;

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            GoogleDriveDownload(mListOfId[3], dTokenList[0]);
        }
        else
        {
            int code;
            QVariantMap error = result["error"].toMap();
            code = error["code"].toInt();
            qDebug() << error["message"].toString();

            if(code == 404)
            {
                download_streams[0] = "";

                // retrieve coordinates:
                (download_streams[0]).append(QByteArray::number(3));

                reply3->deleteLater();

                dctr++;

                emit checkCompleted();
            }
            else if(code == 401)
            {
                SessionGoogleDrive session;
                dTokenList[0] = session.getNewAccessToken(dTokenList[0]);
                dTokenList[0].append("Google Drive");
                TokenIsChanged = true;

                GoogleDriveDownload(mListOfId[3], dTokenList[0]);
            }
            else
            {
                GoogleDriveDownload(mListOfId[3], dTokenList[0]);
            }
        }
    }
    else
    {
        QString downloadUrl;
        QByteArray json;
        json = reply3->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            GoogleDriveDownload(mListOfId[3], dTokenList[0]);
        }
        else
        {
            downloadUrl = result["downloadUrl"].toString();
            QUrl download_url(downloadUrl);
            download_url.addQueryItem("access_token", dTokenList[0][0]);

            QNetworkRequest request(download_url);
            reply3 = manager->get(request);
            connect(reply3, SIGNAL(finished()), this, SLOT(dreplyfinished3a()));
        }
    }
}

void NetworkControllerParallel::dreplyfinished3a()
{
    if(reply3->error())
    {
        qDebug() << reply3->errorString();
        qDebug() << reply3->readAll();

        GoogleDriveDownload(mListOfId[3], dTokenList[0]);
    }
    else
    {
        // qDebug() << reply->rawHeaderPairs();

        download_streams[0] = reply3->readAll();

        // retrieve coordinates:
        (download_streams[0]).append(QByteArray::number(3));

        reply3->deleteLater();

        dctr++;

        emit checkCompleted();
    }
}

void NetworkControllerParallel::GoogleDriveDownload2(QString fileID, QStringList AccessToken)

{
    QUrl get_url(GoogleDrive_download_uri + fileID);
    get_url.addQueryItem("access_token", AccessToken[0]);

    QNetworkRequest request(get_url);
    reply4 = manager->get(request);
    connect(reply4, SIGNAL(finished()), this, SLOT(dreplyfinished4()));
}

void NetworkControllerParallel::dreplyfinished4()
{
    if(reply4->error())
    {
        qDebug() << reply4->errorString();

        QByteArray json;
        json = reply4->readAll();
        qDebug() << json;

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            GoogleDriveDownload2(mListOfId[5], dTokenList[0]);
        }
        else
        {
            int code;
            QVariantMap error = result["error"].toMap();
            code = error["code"].toInt();
            qDebug() << error["message"].toString();

            if(code == 404)
            {
                download_streams[1] = "";

                // retrieve coordinates:
                (download_streams[1]).append(QByteArray::number(5));

                reply4->deleteLater();

                dctr++;

                emit checkCompleted();
            }
            else if(code == 401)
            {
                SessionGoogleDrive session;
                dTokenList[0] = session.getNewAccessToken(dTokenList[0]);
                dTokenList[0].append("Google Drive");
                TokenIsChanged = true;

                GoogleDriveDownload2(mListOfId[5], dTokenList[0]);
            }
            else
            {
                GoogleDriveDownload2(mListOfId[5], dTokenList[0]);
            }
        }
    }
    else
    {
        QString downloadUrl;
        QByteArray json;
        json = reply4->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            GoogleDriveDownload2(mListOfId[5], dTokenList[0]);
        }
        else
        {
            downloadUrl = result["downloadUrl"].toString();
            QUrl download_url(downloadUrl);
            download_url.addQueryItem("access_token", dTokenList[0][0]);

            QNetworkRequest request(download_url);
            reply4 = manager->get(request);
            connect(reply4, SIGNAL(finished()), this, SLOT(dreplyfinished4a()));
        }
    }
}

void NetworkControllerParallel::dreplyfinished4a()
{
    if(reply4->error())
    {
        qDebug() << reply4->errorString();
        qDebug() << reply4->readAll();

        GoogleDriveDownload2(mListOfId[5], dTokenList[0]);
    }
    else
    {
        // qDebug() << reply->rawHeaderPairs();

        download_streams[1] = reply4->readAll();

        // retrieve coordinates:
        (download_streams[1]).append(QByteArray::number(5));

        reply4->deleteLater();

        dctr++;

        emit checkCompleted();
    }
}

void NetworkControllerParallel::SkyDriveDownload(QString fileID, QStringList AccessToken)
{
    QUrl get_url(SkyDrive_download_uri + fileID + "/content/");
    get_url.addQueryItem("access_token", AccessToken[0]);

    QNetworkRequest request(get_url);
    reply5 = manager->get(request);
    connect(reply5, SIGNAL(finished()), this, SLOT(dreplyfinished5()));
}

void NetworkControllerParallel::DownloadLoop(QUrl get_url)
{
    QNetworkRequest request(get_url);
    reply5 = manager->get(request);
    connect(reply5, SIGNAL(finished()), this, SLOT(dreplyfinished5()));
}

void NetworkControllerParallel::dreplyfinished5()
{
    if(reply5->error())
    {
        qDebug() << reply5->errorString();

        QByteArray json;
        json = reply5->readAll();
        qDebug() << json;

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            SkyDriveDownload(mListOfId[2], dTokenList[0]);
        }
        else
        {
            QString code;
            QVariantMap error = result["error"].toMap();
            code = error["code"].toString();
            qDebug() << error["message"].toString();

            if(code.contains("resource_not_found"))
            {
                download_streams[0] = "";

                // retrieve coordinates:
                (download_streams[0]).append(QByteArray::number(2));

                reply5->deleteLater();

                dctr++;

                emit checkCompleted();
            }
            else if(code.contains("request_url_invalid"))
            {
                download_streams[0] = "";

                // retrieve coordinates:
                (download_streams[0]).append(QByteArray::number(2));

                reply5->deleteLater();

                dctr++;

                emit checkCompleted();
            }
            else if(code.contains("request_token_expired"))
            {
                SessionSkyDrive session;
                dTokenList[0] = session.getNewAccessToken(dTokenList[0]);
                dTokenList[0].append("SkyDrive");
                TokenIsChanged = true;

                SkyDriveDownload(mListOfId[2], dTokenList[0]);
            }
            else if(code.contains("request_throttled"))
            {
                QTimer::singleShot(600000, this, SLOT(toSkyDriveDownload()));
            }
            else
            {
                SkyDriveDownload(mListOfId[2], dTokenList[0]);
            }
        }
    }
    else
    {
        if (reply5->hasRawHeader("location"))
        {
            QString downloadUrl = reply5->rawHeader("location");

            QUrl get_url(downloadUrl);
            DownloadLoop(get_url); // recursion here.
        }
        else
        {
            download_streams[0] = reply5->readAll();

            // retrieve coordinates:
            (download_streams[0]).append(QByteArray::number(2));

            reply5->deleteLater();

            dctr++;

            emit checkCompleted();
        }
    }
}

void NetworkControllerParallel::toSkyDriveDownload()
{
    SkyDriveDownload(mListOfId[2], dTokenList[0]);
}

void NetworkControllerParallel::UbuntuOneDownload(QString fileID, QStringList AccessToken)
{
    QUrl get_url = setUbuntuUrl(fileID, file_uri, AccessToken);

    QNetworkRequest request(get_url);
    reply6 = manager->get(request);
    connect(reply6, SIGNAL(finished()), this, SLOT(dreplyfinished6()));
}

void NetworkControllerParallel::dreplyfinished6()
{
    if(reply6->error())
    {
        qDebug() << reply6->errorString();

        QByteArray json;
        json = reply6->readAll();

        if(json.contains("Could not locate object"))
        {
            qDebug() << json;
            download_streams[1] = "";

            // retrieve coordinates:
            (download_streams[1]).append(QByteArray::number(4));

            reply6->deleteLater();

            dctr++;

            emit checkCompleted();
        }
        else
        {
            qDebug() << json;
            UbuntuOneDownload(mListOfId[4], dTokenList[1]);
        }
    }
    else
    {
        // qDebug() << reply->rawHeaderPairs();

        download_streams[1] = reply6->readAll();

        // retrieve coordinates:
        (download_streams[1]).append(QByteArray::number(4));

        reply6->deleteLater();

        dctr++;

        emit checkCompleted();
    }
}

void NetworkControllerParallel::drop(QStringList ListOfId, QStringList *TokenList)
{
    mListOfId = ListOfId;

    mTokenList = new QStringList[3];
    mTokenList = TokenList;

    // do parallel deletings:
    DropboxDelete(ListOfId[0], TokenList[0]);
    DropboxDelete2(ListOfId[1], TokenList[0]);

    GoogleDriveDelete(ListOfId[3], TokenList[1]);
    GoogleDriveDelete2(ListOfId[5], TokenList[1]);

    SkyDriveDelete(ListOfId[2], TokenList[2]);
    UbuntuOneDelete(ListOfId[4], TokenList[3]);
}

void NetworkControllerParallel::DropboxDelete(QString Source, QStringList AccessToken)
{
    QUrl post_url = setDropboxUrl("", Dropbox_delete_uri, AccessToken);
    post_url.addQueryItem("root", "sandbox");
    post_url.addQueryItem("path", Source.remove("/"));
    post_url.removeQueryItem("uid");

    QNetworkRequest request(post_url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");

    reply1 = manager->post(request, "");
    connect(reply1, SIGNAL(finished()), this, SLOT(dereplyfinished1()));
}

void NetworkControllerParallel::dereplyfinished1()
{
    if(reply1->error())
    {
        qDebug() << reply1->errorString();

        QByteArray json;
        json = reply1->readAll();
        qDebug() << json;

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            DropboxDelete(mListOfId[0], mTokenList[0]);
        }
        else
        {
            QString error = result["error"].toString();
            qDebug() << error;

            if(error.contains("deleted"))
            {
                reply1->deleteLater();

                dectr++;

                emit checkCompleted();
            }
            else if(error.contains("not found"))
            {
                reply1->deleteLater();

                dectr++;

                emit checkCompleted();
            }
            else if(error.contains("Path must not be empty"))
            {
                reply1->deleteLater();

                dectr++;

                emit checkCompleted();
            }
            else
            {
                DropboxDelete(mListOfId[0], mTokenList[0]);
            }
        }
    }
    else
    {
        QByteArray json;
        json = reply1->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            DropboxDelete(mListOfId[0], mTokenList[0]);
        }
        else
        {
            bool success;
            success = result["is_deleted"].toBool();

            if(success)
            {
                // return true;
            }
            else
            {
                // return false;
            }

            reply1->deleteLater();

            dectr++;

            emit checkCompleted();
        }
    }
}

void NetworkControllerParallel::DropboxDelete2(QString Source, QStringList AccessToken)
{
    QUrl post_url = setDropboxUrl("", Dropbox_delete_uri, AccessToken);
    post_url.addQueryItem("root", "sandbox");
    post_url.addQueryItem("path", Source.remove("/"));
    post_url.removeQueryItem("uid");

    QNetworkRequest request(post_url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/x-www-form-urlencoded");

    reply2 = manager->post(request, "");
    connect(reply2, SIGNAL(finished()), this, SLOT(dereplyfinished2()));
}

void NetworkControllerParallel::dereplyfinished2()
{
    if(reply2->error())
    {
        qDebug() << reply2->errorString();

        QByteArray json;
        json = reply2->readAll();
        qDebug() << json;

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            DropboxDelete2(mListOfId[1], mTokenList[0]);
        }
        else
        {
            QString error = result["error"].toString();
            qDebug() << error;

            if(error.contains("deleted"))
            {
                reply2->deleteLater();

                dectr++;

                emit checkCompleted();
            }
            else if(error.contains("not found"))
            {
                reply2->deleteLater();

                dectr++;

                emit checkCompleted();
            }
            else if(error.contains("Path must not be empty"))
            {
                reply2->deleteLater();

                dectr++;

                emit checkCompleted();
            }
            else
            {
                DropboxDelete2(mListOfId[1], mTokenList[0]);
            }
        }
    }
    else
    {
        QByteArray json;
        json = reply2->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            DropboxDelete2(mListOfId[1], mTokenList[0]);
        }
        else
        {
            bool success;
            success = result["is_deleted"].toBool();

            if(success)
            {
                // return true;
            }
            else
            {
                // return false;
            }

            reply2->deleteLater();

            dectr++;

            emit checkCompleted();
        }
    }
}

void NetworkControllerParallel::GoogleDriveDelete(QString fileID, QStringList AccessToken)
{
    QUrl delete_url(GoogleDrive_download_uri + fileID);
    delete_url.addQueryItem("access_token", AccessToken[0]);

    QNetworkRequest request(delete_url);
    reply3 = manager->deleteResource(request);
    connect(reply3, SIGNAL(finished()), this, SLOT(dereplyfinished3()));
}

void NetworkControllerParallel::dereplyfinished3()
{
    if(reply3->error())
    {
        qDebug() << reply3->errorString();

        QByteArray json;
        json = reply3->readAll();
        qDebug() << json;

        if(json.contains("Not Found"))
        {
            reply3->deleteLater();

            dectr++;

            emit checkCompleted();
        }
        else
        {
            QJson::Parser parser;
            bool ok;

            QVariantMap result = parser.parse(json, &ok).toMap();
            if (!ok)
            {
                qFatal("An error occurred during parsing");

                GoogleDriveDelete(mListOfId[3], mTokenList[1]);
            }
            else
            {
                int code;
                QVariantMap error = result["error"].toMap();
                code = error["code"].toInt();
                qDebug() << error["message"].toString();

                if(code == 404)
                {
                    reply3->deleteLater();

                    dectr++;

                    emit checkCompleted();
                }
                else if(code == 401)
                {
                    SessionGoogleDrive session;
                    mTokenList[1] = session.getNewAccessToken(mTokenList[1]);
                    mTokenList[1].append("Google Drive");
                    TokenIsChanged = true;

                    GoogleDriveDelete(mListOfId[3], mTokenList[1]);
                }
                else
                {
                    GoogleDriveDelete(mListOfId[3], mTokenList[1]);
                }
            }
        }
    }
    else
    {
        if(reply3->readAll().contains(""))
        {
            // return true;
        }
        else
        {
            // return false;
        }

        reply3->deleteLater();

        dectr++;

        emit checkCompleted();
    }
}

void NetworkControllerParallel::GoogleDriveDelete2(QString fileID, QStringList AccessToken)
{
    QUrl delete_url(GoogleDrive_download_uri + fileID);
    delete_url.addQueryItem("access_token", AccessToken[0]);

    QNetworkRequest request(delete_url);
    reply4 = manager->deleteResource(request);
    connect(reply4, SIGNAL(finished()), this, SLOT(dereplyfinished4()));
}

void NetworkControllerParallel::dereplyfinished4()
{
    if(reply4->error())
    {
        qDebug() << reply4->errorString();

        QByteArray json;
        json = reply4->readAll();
        qDebug() << json;

        if(json.contains("Not Found"))
        {
            reply4->deleteLater();

            dectr++;

            emit checkCompleted();
        }
        else
        {
            QJson::Parser parser;
            bool ok;

            QVariantMap result = parser.parse(json, &ok).toMap();
            if (!ok)
            {
                qFatal("An error occurred during parsing");

                GoogleDriveDelete2(mListOfId[5], mTokenList[1]);
            }
            else
            {
                int code;
                QVariantMap error = result["error"].toMap();
                code = error["code"].toInt();
                qDebug() << error["message"].toString();

                if(code == 404)
                {
                    reply4->deleteLater();

                    dectr++;

                    emit checkCompleted();
                }
                else if(code == 401)
                {
                    SessionGoogleDrive session;
                    mTokenList[1] = session.getNewAccessToken(mTokenList[1]);
                    mTokenList[1].append("Google Drive");
                    TokenIsChanged = true;

                    GoogleDriveDelete2(mListOfId[5], mTokenList[1]);
                }
                else
                {
                    GoogleDriveDelete2(mListOfId[5], mTokenList[1]);
                }
            }
        }
    }
    else
    {
        if(reply4->readAll().contains(""))
        {
            // return true;
        }
        else
        {
            // return false;
        }

        reply4->deleteLater();

        dectr++;

        emit checkCompleted();
    }
}

void NetworkControllerParallel::SkyDriveDelete(QString fileID, QStringList AccessToken)
{
    QUrl delete_url(SkyDrive_download_uri + fileID);
    delete_url.addQueryItem("access_token", AccessToken[0]);

    QNetworkRequest request(delete_url);
    reply5 = manager->deleteResource(request);
    connect(reply5, SIGNAL(finished()), this, SLOT(dereplyfinished5()));
}

void NetworkControllerParallel::dereplyfinished5()
{
    if(reply5->error())
    {
        qDebug() << reply5->errorString();

        QByteArray json;
        json = reply5->readAll();
        qDebug() << json;

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            SkyDriveDelete(mListOfId[2], mTokenList[2]);
        }
        else
        {
            QString code, message;
            QVariantMap error = result["error"].toMap();
            code = error["code"].toString();
            message = error["message"].toString();
            qDebug() << message;

            if(code.contains("resource_not_found"))
            {
                reply5->deleteLater();

                dectr++;

                emit checkCompleted();
            }
            else if(code.contains("request_url_invalid"))
            {
                reply5->deleteLater();

                dectr++;

                emit checkCompleted();
            }
            else if(code.contains("request_throttled"))
            {
                QTimer::singleShot(600000, this, SLOT(toSkyDriveDelete()));
            }
            else if(code.contains("request_token_expired"))
            {
                SessionSkyDrive session;
                mTokenList[2] = session.getNewAccessToken(mTokenList[2]);
                TokenIsChanged = true;

                SkyDriveDelete(mListOfId[2], mTokenList[2]);
            }
            else
            {
                if(message.contains("The URL doesn't contain a resource."))
                {
                    reply5->deleteLater();

                    dectr++;

                    emit checkCompleted();
                }
                else
                {
                    qDebug() << message;

                    SkyDriveDelete(mListOfId[2], mTokenList[2]);
                }
            }
        }
    }
    else
    {
        if(reply5->readAll().contains(""))
        {
            // return true;
        }
        else
        {
            // return false;
        }

        reply5->deleteLater();

        dectr++;

        emit checkCompleted();
    }
}

void NetworkControllerParallel::toSkyDriveDelete()
{
    SkyDriveDelete(mListOfId[2], mTokenList[2]);
}

void NetworkControllerParallel::UbuntuOneDelete(QString fileID, QStringList AccessToken)
{
    fileID.remove("/content/~/Ubuntu One/");
    QUrl delete_url = setUbuntuUrl(fileID, base_uri + api + volume, AccessToken);

    QNetworkRequest request(delete_url);
    reply6 = manager->deleteResource(request);
    connect(reply6, SIGNAL(finished()), this, SLOT(dereplyfinished6()));
}

void NetworkControllerParallel::dereplyfinished6()
{
    if(reply6->error())
    {
        qDebug() << reply6->errorString();

        QByteArray json;
        json = reply6->readAll();
        qDebug() << json;

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            UbuntuOneDelete(mListOfId[4], mTokenList[3]);
        }
        else
        {
            QString error = result["error"].toString();
            qDebug() << error;

            if(error.contains("The provided Node id does not exist."))
            {
                reply6->deleteLater();

                dectr++;

                emit checkCompleted();
            }
            else
            {
                UbuntuOneDelete(mListOfId[4], mTokenList[3]);
            }
        }
    }
    else
    {
        if(reply6->readAll().contains(""))
        {
            //qDebug() << "deleted!";
        }
        else
        {
            qDebug() << reply6->readAll();
        }

        reply6->deleteLater();

        dectr++;

        emit checkCompleted();
    }
}

QStringList NetworkControllerParallel::createBackup(QStringList *TokenList)
{
    QStringList IdList;
    QString id2, id3, id4;

    id2 = GoogleDriveCreateBackup(TokenList[1]);
    IdList.append(id2);

    id3 = SkyDriveCreateBackup(TokenList[2]);
    IdList.append(id3);

    id4 = UbuntuOneCreateBackup(TokenList[3]);
    IdList.append(id4);

    return IdList;
}

QString NetworkControllerParallel::GoogleDriveCreateBackup(QStringList AccessToken)
{
    QString id;

    QUrl folder_url(GoogleDrive_folder_uri);
    folder_url.addQueryItem("access_token", AccessToken[0]);

    QByteArray metadata("");
    metadata.append("{\n");
    metadata.append("  \"title\": \"HSRC_backup\",\n");
    metadata.append("  \"mimeType\": \"application/vnd.google-apps.folder\"\n");
    metadata.append("}");

    QNetworkRequest request(folder_url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/json; charset=UTF-8");
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      QString::number(metadata.size()));
    reply = manager->post(request, metadata);
    // use an event loop to control the signal:
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if(reply->error())
    {
        qDebug() << reply->errorString();
        qDebug() << reply->readAll();

        reply->close();
        folder_url.clear();

        return GoogleDriveCreateBackup(AccessToken);
    }
    else
    {
        QByteArray json;
        json = reply->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            reply->close();
            folder_url.clear();

            return GoogleDriveCreateBackup(AccessToken);
        }
        else
        {
            reply->close();
            folder_url.clear();

            id = result["id"].toString();
            return id;
        }
    }
}

QString NetworkControllerParallel::SkyDriveCreateBackup(QStringList AccessToken)
{
    QString id;

    QUrl folder_url(SkyDrive_folder_uri);
    folder_url.addQueryItem("access_token", AccessToken[0]);

    QByteArray metadata("");
    metadata.append("{\n");
    metadata.append("  \"name\": \"HSRC_backup\"\n");
    metadata.append("}");

    QNetworkRequest request(folder_url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,
                      "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader,
                      QString::number(metadata.size()));
    reply = manager->post(request, metadata);
    // use an event loop to control the signal:
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if(reply->error())
    {
        qDebug() << reply->errorString();
        qDebug() << reply->readAll();

        reply->close();
        folder_url.clear();

        return SkyDriveCreateBackup(AccessToken);
    }
    else
    {
        QByteArray json;
        json = reply->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            reply->close();
            folder_url.clear();

            return SkyDriveCreateBackup(AccessToken);
        }
        else
        {
            reply->close();
            folder_url.clear();

            id = result["id"].toString();
            return id;
        }
    }
}

QString NetworkControllerParallel::UbuntuOneCreateBackup(QStringList AccessToken)
{
    QString resource_path;

    QUrl put_url = setUbuntuUrl("", base_uri + api + volume + "HSRC_backup/",
                                AccessToken);

    QNetworkRequest request(put_url);

    QByteArray metadata("");
    metadata.append("{\n");
    metadata.append("  \"kind\": \"").append("directory").append("\"\n");
    metadata.append("}");

    reply = manager->put(request, metadata);
    // use an event loop to control the signal:
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if(reply->error())
    {
        qDebug() << reply->errorString();
        qDebug() << reply->readAll();

        reply->close();
        put_url.clear();

        return UbuntuOneCreateBackup(AccessToken);
    }
    else
    {
        QByteArray json;
        json = reply->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            reply->close();
            put_url.clear();

            return UbuntuOneCreateBackup(AccessToken);
        }
        else
        {
            reply->close();
            put_url.clear();

            resource_path = result["resource_path"].toString();
            return resource_path;
        }
    }
}

QStringList NetworkControllerParallel::check(QStringList ListOfId, QStringList *TokenList)
{
    QStringList file_status;

    file_status.append(DropboxCheckFile(ListOfId[0], TokenList[0]));
    file_status.append(DropboxCheckFile(ListOfId[1], TokenList[0]));

    file_status.append(GoogleDriveCheckFile(ListOfId[3], TokenList[1]));
    file_status.append(GoogleDriveCheckFile(ListOfId[5], TokenList[1]));

    file_status.append(UbuntuOneCheckFile(ListOfId[2], TokenList[3]));
    file_status.append(UbuntuOneCheckFile(ListOfId[4], TokenList[3]));
    // file_status.append(SkyDriveCheckFile(ListOfId[2], TokenList[2]));
    // file_status.append(SkyDriveCheckFile(ListOfId[4], TokenList[2]));

    return file_status;
}

QString NetworkControllerParallel::DropboxCheckFile(QString Source,
                                                    QStringList AccessToken)
{
    QString file_status;

    QUrl get_url = setDropboxUrl(Source, Dropbox_metadata_uri, AccessToken);
    get_url.addQueryItem("hash", "");
    get_url.removeQueryItem("uid");

    QNetworkRequest request(get_url);
    reply = manager->get(request);

    // use an event loop to control the signal:
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if(reply->error())
    {
        qDebug() << reply->errorString();
        qDebug() << reply->readAll();

        reply->close();
        get_url.clear();

        return DropboxCheckFile(Source, AccessToken);
    }
    else
    {
        QByteArray json;
        json = reply->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            reply->close();
            get_url.clear();

            return DropboxCheckFile(Source, AccessToken);
        }
        else
        {
            reply->close();
            get_url.clear();

            if(result.contains("is_deleted"))
            {
                if(result["is_deleted"].toBool())
                {
                    file_status = "NotFound";
                    return file_status;
                }
                else
                {
                    file_status = "Found";
                    return file_status;
                }
            }
            else
            {
                // qDebug() << result["rev"].toString(); // for temp!

                // write a routine for checking the changes:
                file_status = "Found";
                return file_status;
            }
        }
    }
}

QString NetworkControllerParallel::GoogleDriveCheckFile(QString fileID,
                                                        QStringList AccessToken)
{
    QString file_status;
    int code;

    QUrl check_url(GoogleDrive_download_uri + fileID);
    check_url.addQueryItem("access_token", AccessToken[0]);

    QNetworkRequest request(check_url);
    reply = manager->get(request);

    // use an event loop to control the signal:
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if(reply->error())
    {
        qDebug() << reply->errorString();

        QByteArray json;
        json = reply->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            reply->close();
            check_url.clear();

            return GoogleDriveCheckFile(fileID, AccessToken);
        }
        else
        {
            reply->close();
            check_url.clear();

            QVariantMap error = result["error"].toMap();
            code = error["code"].toInt();

            if(code == 404)
            {
                qDebug() << error["message"].toString();
                file_status = "NotFound";
                return file_status;
            }
            else
            {
                qDebug() << error["message"].toString();
                return GoogleDriveCheckFile(fileID, AccessToken);
            }
        }
    }
    else
    {
        QByteArray json;
        json = reply->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            reply->close();
            check_url.clear();

            return GoogleDriveCheckFile(fileID, AccessToken);
        }
        else
        {
            reply->close();
            check_url.clear();

            if(result["trashed"].toBool())
            {
                // smtg wrong here!

                file_status = "NotFound";
                return file_status;
            }
            else
            {
                // write a routine to handle changes:


                file_status = "Found";
                return file_status;
            }
        }
    }
}

QString NetworkControllerParallel::UbuntuOneCheckFile(QString Source,
                                                      QStringList AccessToken)
{
    QString file_status;

    QUrl check_url = setUbuntuUrl(Source, base_uri + api + volume, AccessToken);

    QNetworkRequest request(check_url);
    reply = manager->get(request);

    // use an event loop to control the signal:
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if(reply->error())
    {
        qDebug() << reply->errorString();
        qDebug() << reply->readAll();

        reply->close();
        check_url.clear();

        return UbuntuOneCheckFile(Source, AccessToken);
    }
    else
    {
        qDebug() << "here";
        qDebug() << reply->rawHeaderList();
        qDebug() << reply->rawHeaderPairs();
        qDebug() << reply->readAll();

        reply->close();
        check_url.clear();

        file_status = "NotFound";
        return file_status; // tbd!!!
    }
}

QString NetworkControllerParallel::SkyDriveCheckFile(QString fileID,
                                                     QStringList AccessToken)
{
    QString file_status;
    QString code;

    QUrl check_url(SkyDrive_download_uri + fileID);
    check_url.addQueryItem("access_token", AccessToken[0]);

    QNetworkRequest request(check_url);
    reply = manager->get(request);

    // use an event loop to control the signal:
    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if(reply->error())
    {
        qDebug() << reply->errorString();

        QByteArray json;
        json = reply->readAll();

        QJson::Parser parser;
        bool ok;

        QVariantMap result = parser.parse(json, &ok).toMap();
        if (!ok)
        {
            qFatal("An error occurred during parsing");

            reply->close();
            check_url.clear();

            return SkyDriveCheckFile(fileID, AccessToken);
        }
        else
        {
            reply->close();
            check_url.clear();

            QVariantMap error = result["error"].toMap();
            code = error["code"].toString();

            if(code.contains("resource_not_found"))
            {
                qDebug() << error["message"].toString();
                file_status = "NotFound";
                return file_status;
            }
            else
            {
                qDebug() << error["message"].toString();
                return SkyDriveCheckFile(fileID, AccessToken);
            }
        }
    }
    else
    {
        // qDebug() << reply->readAll();

        reply->close();
        check_url.clear();

        // write a routine for checking the changes:
        file_status = "Found";
        return file_status;

    }
}

void NetworkControllerParallel::Completed()
{
    if(ctr == 6)
    {
        for(int i = 0; i < N; i++)
            delete[] encoded_parts[i];

        delete encoded_parts;

        emit completed();
    }

    if(dctr == 2)
    {
        emit completed();
    }

    if(dectr == 6)
    {
        emit completed();
    }
}

QStringList* NetworkControllerParallel::getTokenList()
{
    return mTokenList;
}

QStringList* NetworkControllerParallel::getDecodingTokenList()
{
    return dTokenList;
}



