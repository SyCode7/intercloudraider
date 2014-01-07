#include "enginecryptographichashing.h"

EngineCryptographicHashing::EngineCryptographicHashing()
{
}

// Calculate md5 for chunks indexing.
QString EngineCryptographicHashing::calculateHash(QByteArray chunk_stream)
{
    QCryptographicHash crypto(QCryptographicHash::Md5);
    crypto.addData(chunk_stream);
    QByteArray hash = crypto.result();
    return hash.toHex();
}

// Calculate MD5.
QByteArray EngineCryptographicHashing::calculateMd5(QByteArray byte_stream)
{
    QCryptographicHash crypto(QCryptographicHash::Md5);
    crypto.addData(byte_stream);
    QByteArray hash = crypto.result();
    return hash;
}

// Calculate SHA1.
QByteArray EngineCryptographicHashing::calculateSha1(QByteArray byte_stream)
{
    QCryptographicHash crypto(QCryptographicHash::Sha1);
    crypto.addData(byte_stream);
    QByteArray hash = crypto.result();
    return hash;
}

// Calculate md4 for masking names.
QString EngineCryptographicHashing::calculateHash(QString chunk_name)
{
    QCryptographicHash crypto(QCryptographicHash::Md4);
    crypto.addData(chunk_name.toStdString().c_str(), chunk_name.size());
    QByteArray hash = crypto.result();
    return hash.toHex();
}
