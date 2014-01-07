#ifndef ENGINEPDP_H
#define ENGINEPDP_H

#include "PolarSSL/entropy.h"
#include "PolarSSL/ctr_drbg.h"
#include "PolarSSL/aes.h"
#include "CoreEngines/enginecryptographichashing.h"
#include "DBContr/databasecontrollerengines.h"
#include "DBContr/databasecontrollersetting.h"
#include "CoreEngines/enginecoding.h"

#include <QString>
#include <QDebug>

class EnginePDP
{
    static const int MD5_HMAC_BLOCKSIZE = 64; // in bytes
    static const int MD5_HMAC_KEYSIZE = 16;

    // By using the formula 1 - (1 - m/d)^r, where m is the number of
    // lost or modified chunks and d is the total number of chunks, we
    // compute the detection probability.
    // If m/d = 0.01 and r = 512, we can get 99% detection probability.
    static const int TOTAL_COVERED_HASHES = 512; // TOTAL_COVERED_HASHES = r

    QByteArray MD5_HMAC(unsigned char *key, unsigned char *token_index);
    QByteArray AES_ECB(unsigned char *hash_index, QByteArray session_key);
    unsigned char* KeyGenerator();
    int CorePermutator(int position, int d, QString result,
                       QList<int> mList, QList<int> List,
                       QByteArray session_key);
    int IndexPermutator(QByteArray session_key, int index, int d,
                        QList<int> mList, QList<int> List);
    QByteArray retrieveMd5(int index, QStringList *TokenList,
                           QStringList key_secret, QStringList folder_id);

    bool isChanged;
    QStringList *DecodingTokenList;

public:
    static const int TOTAL_TOKENS = 10;

    EnginePDP();

    QStringList PDPSetup();
    bool PDPVerification(QStringList *TokenList, int i);

    // for tokens:
    bool getIsChanged();
    QStringList* getDecodingTokenList();
};

#endif // ENGINEPDP_H
