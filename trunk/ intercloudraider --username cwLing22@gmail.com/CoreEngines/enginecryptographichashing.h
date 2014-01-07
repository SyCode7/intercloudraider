#ifndef ENGINECRYPTOGRAPHICHASHING_H
#define ENGINECRYPTOGRAPHICHASHING_H

#include <QCryptographicHash>
#include <QString>

class EngineCryptographicHashing
{
public:
    EngineCryptographicHashing();

    QString calculateHash(QByteArray chunk_stream);

    QByteArray calculateMd5(QByteArray byte_stream);

    QByteArray calculateSha1(QByteArray byte_stream);

    QString calculateHash(QString chunk_name);
};

#endif // ENGINECRYPTOGRAPHICHASHING_H
