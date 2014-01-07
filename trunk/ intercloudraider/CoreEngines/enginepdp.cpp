#include "enginepdp.h"

EnginePDP::EnginePDP()
{
}

QByteArray EnginePDP::MD5_HMAC(unsigned char *key, unsigned char *token_index)
{
    unsigned char *opad, *ipad;
    opad = new unsigned char[MD5_HMAC_BLOCKSIZE];
    ipad = new unsigned char[MD5_HMAC_BLOCKSIZE];

    for(int i = 0; i < MD5_HMAC_BLOCKSIZE; i++)
    {
        opad[i] = 0x5c;
        ipad[i] = 0x36;
    }

    QByteArray o_key_pad, i_key_pad, q_index;

    unsigned char *k;
    k = new unsigned char[MD5_HMAC_BLOCKSIZE];

    for(int i = 0; i < MD5_HMAC_BLOCKSIZE; i++)
    {
        if(i < MD5_HMAC_KEYSIZE)
        {
            k[i] = key[i];
        }
        else
        {
            k[i] = 0x00;
        }
    }

    for(int i = 0; i < MD5_HMAC_BLOCKSIZE; i++)
    {
        opad[i] ^= k[i];
        ipad[i] ^= k[i];
    }

    for(int i = 0; i < MD5_HMAC_BLOCKSIZE; i++)
    {
        i_key_pad.append(ipad[i]);
        o_key_pad.append(opad[i]);
    }

    for(int i = 0; i < MD5_HMAC_KEYSIZE; i++)
    {
        q_index.append(token_index[i]);
    }

    EngineCryptographicHashing hashing;
    QByteArray result = hashing.calculateMd5(o_key_pad.append(hashing
                        .calculateMd5(i_key_pad.append(q_index))));

    return result;
}

unsigned char* EnginePDP::KeyGenerator()
{
    ctr_drbg_context ctr_drbg;
    entropy_context entropy;
    unsigned char *key; // 128bits key
    key = new unsigned char[16];

    char *pers = "aes_key_generation";
    int ret;

    // Generate master key W (or Z) by random generator drbg:
    entropy_init(&entropy);
    if((ret = ctr_drbg_init(&ctr_drbg, entropy_func, &entropy, (unsigned char *)pers,
                            strlen(pers))) != 0)
    {
        printf(" failed\n ! ctr_drbg_init returned -0x%04x\n", -ret);
        exit(EXIT_FAILURE);
    }

    if((ret = ctr_drbg_random(&ctr_drbg, key, 16)) != 0)
    {
        printf(" failed\n ! ctr_drbg_random returned -0x%04x\n", -ret);
        exit(EXIT_FAILURE);
    }

    return key;
}

QByteArray EnginePDP::AES_ECB(unsigned char *hash_index, QByteArray session_key)
{
    QByteArray result;

    aes_context aes;
    unsigned char output[16];

    aes_setkey_enc(&aes, reinterpret_cast<unsigned char*>(session_key.data()), 128);
    aes_crypt_ecb(&aes, AES_ENCRYPT, hash_index, output);

    for(int i = 0; i < 16; i++)
        result.append(output[i]);

    return result;
}

int EnginePDP::CorePermutator(int position, int d, QString result,
                              QList<int> mList, QList<int> List,
                              QByteArray session_key)
{ 
    int permutated_index, permutated_index1;
    QString qpermutated_index = result.left(position);
    bool ok;
    permutated_index1 = qpermutated_index.toULongLong(&ok, 16);
    permutated_index = permutated_index1 % d;

    if(!mList.isEmpty())
    {
        if(mList.contains(permutated_index))
        {
            permutated_index++;
        }
    }

    if(List.contains(permutated_index))
    {
        int permutated_index2;
        mList.append(permutated_index);
        QString qpermutated_index = result.right(position);
        bool ok;
        permutated_index2 = qpermutated_index.toULongLong(&ok, 16);
        permutated_index = permutated_index2 % d;

        if(!mList.isEmpty())
        {
            if(mList.contains(permutated_index))
            {
                permutated_index++;
            }
        }

        if(List.contains(permutated_index))
        {
            int permutated_index3;
            mList.append(permutated_index);
            QString qpermutated_index = result.right(1 + position);
            bool ok;
            permutated_index3 = qpermutated_index.toULongLong(&ok, 16);
            permutated_index = permutated_index3 % d;

            if(!mList.isEmpty())
            {
                if(mList.contains(permutated_index))
                {
                    permutated_index++;
                }
            }

            if(List.contains(permutated_index))
            {
                mList.append(permutated_index);
                permutated_index = IndexPermutator(session_key, permutated_index,
                                                   d, mList, List);
            }
        }
    }

    return permutated_index;
}

int EnginePDP::IndexPermutator(QByteArray session_key, int index, int d,
                               QList<int> mList, QList<int> List)
{
    uint64_t power = 1;
    int permutated_index;

    unsigned char *c_index;
    c_index = new unsigned char[16];
    QString q_index, result;
    q_index.setNum(index, 10);

    // Prepend (16 - length) many "0"s string to q_index:
    int length = q_index.size();

    if(length < 16)
    {
        for(int i = 0; i < 16 - length; i++)
            q_index.prepend("0");
    }

    for(int i = 0; i < 16; i++)
    {
        c_index[i] = q_index.at(i).toAscii();
    }

    result = AES_ECB(c_index, session_key).toHex();
    delete[] c_index;

    // 1. Check index falls in which interval [2^8i, 2^8(i+1)), i count from 0.
    // 2. Then take the first left 2(i+1) bytes from AES_ECB(index, session_key).
    // Set permutated_index to this.
    // 3. If permutated_index > d, run IndexPermutator(permutated_index) again
    // and set permutated_index to its output;
    // 4. If List contains permutated_index, run IndexPermutator(permutated_index)
    // again and set permutated_index to its output;
    // 5. Else, return permutated_index.
    if(d < (power << 8))
    {
        permutated_index = CorePermutator(2, d, result, mList, List, session_key);
    }
    else if((power << 8) <= d && d < (power << 12))
    {
        permutated_index = CorePermutator(3, d, result, mList, List, session_key);
    }
    else if((power << 12) <= d && d < (power << 16))
    {
        permutated_index = CorePermutator(4, d, result, mList, List, session_key);
    }
    else if((power << 16) <= d && d < (power << 20))
    {
        permutated_index = CorePermutator(5, d, result, mList, List, session_key);
    }
    else if((power << 20) <= d && d < (power << 24))
    {
        permutated_index = CorePermutator(6, d, result, mList, List, session_key);
    }
    else if((power << 24) <= d && d < (power << 28))
    {
        permutated_index = CorePermutator(7, d, result, mList, List, session_key);
    }
    else if((power << 28) <= d && d < (power << 32))
    {
        permutated_index = CorePermutator(8, d, result, mList, List, session_key);
    }
    else
    {
        qDebug() << "The index is out of range!";
        permutated_index = -1;
    }

    return permutated_index;
}

QStringList EnginePDP::PDPSetup()
{
    QStringList tokens;

    DatabaseControllerEngines dbcontr_e;
    int d = dbcontr_e.getTotalChunks();

    DatabaseControllerSetting dbcontr_s;
    dbcontr_s.deletePreviousTotalChunks();
    dbcontr_s.createPreviousTotalChunks(d);

    // Generate master key W (or Z):
    unsigned char* w_key;
    w_key = new unsigned char[16];
    unsigned char* z_key;
    z_key = new unsigned char[16];

    w_key = KeyGenerator();
    z_key = KeyGenerator();

     QByteArray q_wkey, q_zkey;
     for(int i = 0; i < 16; i++)
     {
         q_wkey.append(w_key[i]);
         q_zkey.append(z_key[i]);
     }

     tokens.append(q_wkey.toHex());
     tokens.append(q_zkey.toHex());

    for(int i = 0; i < TOTAL_TOKENS; i++)
    {
        // Compute i-th session key and challenge nonce by MD5_HMAC:
        unsigned char *c_index;
        c_index = new unsigned char[16];
        QString q_index;
        q_index.setNum(i, 10);

        // Prepend (16 - length) many "0"s string to q_index:
        int length = q_index.size();
        if(length < 16)
        {
            for(int i = 0; i < 16 - length; i++)
                q_index.prepend("0");
        }

        for(int i = 0; i < 16; i++)
            c_index[i] = q_index.at(i).toAscii();

        QByteArray session_key = MD5_HMAC(w_key, c_index),
                   challenge_nonce = MD5_HMAC(z_key, c_index);
        delete[] c_index;

        QByteArray stream;
        stream.append(challenge_nonce);

        if(d <= TOTAL_COVERED_HASHES)
        {
            // If the total number of chunks is no greater than r, attach
            // all the md5 of chunks in MD5_LOG in the verfication token.
            for(int j = 1; j <= d; j++) // count from 1.
            {
                QString md5 = dbcontr_e.getMD5(j);
                QByteArray encoded_md5;
                encoded_md5.append(md5);

                QByteArray decoded_md5 = QByteArray::fromHex(encoded_md5);
                stream.append(decoded_md5);
            }
        }
        else
        {
            QList<int> List, mList;

            for(int j = 0; j < TOTAL_COVERED_HASHES; j++)
            {
                int permutated_index;
                permutated_index = IndexPermutator(session_key, j, d, mList, List); // j =  index
                List.append(permutated_index);

                // Retrieve the md5 that is corresponding to the permutated_seq_no.
                QString md5 = dbcontr_e.getMD5(permutated_index + 1);
                QByteArray encoded_md5;
                encoded_md5.append(md5);

                QByteArray decoded_md5 = QByteArray::fromHex(encoded_md5);
                stream.append(decoded_md5);
            }
        }

        EngineCryptographicHashing hashing;
        QByteArray sha1 = hashing.calculateSha1(stream);

        tokens.append(sha1.toHex());
    }

    delete[] w_key;
    delete[] z_key;

    return tokens;
}

QByteArray EnginePDP::retrieveMd5(int index, QStringList *TokenList,
                                  QStringList key_secret, QStringList folder_id)
{
    QByteArray hash;

    DatabaseControllerEngines dbcontr_e;
    QStringList InfoList = dbcontr_e.retrieveInfo(index);
    int size = dbcontr_e.getSize(index);

    // Download encoded pieces and decode them:
    EngineCoding decoding;
    decoding.createAugmentedMatrix(size, InfoList, TokenList,
                                   key_secret, folder_id);
    decoding.decodeChunks(size);

    if(decoding.TokenIsChanged)
    {
        isChanged = true;
        DecodingTokenList = decoding.DecodingTokenList;
        TokenList = decoding.DecodingTokenList;
    }
    else
    {
        DecodingTokenList = TokenList;
    }

    int chunk_size = dbcontr_e.getChunkSize(index);
    QByteArray bytestream;
    for(int i = 0; i < chunk_size; i++)
        bytestream.append(decoding.de_info.recovery_vector[i]);

    EngineCryptographicHashing hashing;
    hash = hashing.calculateMd5(bytestream);

    delete[] decoding.de_info.recovery_vector;
    InfoList.clear();

    return hash;
}

bool EnginePDP::PDPVerification(QStringList *TokenList, int i)
{
    // Retrieve w_key and z_key:
    DatabaseControllerSetting dbcontr_s;
    QStringList keys = dbcontr_s.retrievePDPkeys();

    QStringList key_secret, folder_id;
    key_secret = dbcontr_s.retrieveKeySecret();
    if(dbcontr_s.checkBackupFolder())
    {
        folder_id = dbcontr_s.retrieveFolderId();
    }

    QByteArray encoded_qw, encoded_qz;
    encoded_qw.append(keys[0]);
    encoded_qz.append(keys[1]);

    QByteArray qw, qz;
    qw = QByteArray::fromHex(encoded_qw);
    qz = QByteArray::fromHex(encoded_qz);

    unsigned char* w_key;
    w_key = new unsigned char[16];
    unsigned char* z_key;
    z_key = new unsigned char[16];

    for(int k = 0; k < 16; k++)
    {
        w_key[k] = qw.at(k);
        z_key[k] = qz.at(k);
    }

    // Compute i-th session key and challenge nonce by MD5_HMAC:
    unsigned char *c_index;
    c_index = new unsigned char[16];
    QString q_index;
    q_index.setNum(i, 10);

    // Prepend (16 - length) many "0"s string to q_index:
    int length = q_index.size();
    if(length < 16)
    {
        for(int i = 0; i < 16 - length; i++)
            q_index.prepend("0");
    }

    for(int k = 0; k < 16; k++)
        c_index[k] = q_index.at(k).toAscii();

    QByteArray session_key = MD5_HMAC(w_key, c_index),
               challenge_nonce = MD5_HMAC(z_key, c_index);

    delete[] c_index;
    delete[] w_key;
    delete[] z_key;

    QByteArray stream;
    stream.append(challenge_nonce);

    int d = dbcontr_s.retrievePreviousTotalChunks();
    if(d <= TOTAL_COVERED_HASHES)
    {
        QProgressDialog progress(QString("Verifying..."), "Cancel", 0, d);
        progress.setWindowFlags(Qt::WindowTitleHint);
        progress.setWindowTitle("PDP Verification");
        progress.setCancelButton(NULL);
        progress.setValue(0);

        // If the total number of chunks is no greater than r, attach
        // all the md5 of chunks in MD5_LOG in the verfication token.
        for(int j = 1; j <= d; j++) // count from 1.
        {   
            QByteArray md5 = retrieveMd5(j, TokenList, key_secret, folder_id);
            stream.append(md5);

            progress.setValue(j);
        }
    }
    else
    {
        QProgressDialog progress(QString("Verifying..."), "Cancel", 0, TOTAL_COVERED_HASHES);
        progress.setWindowFlags(Qt::WindowTitleHint);
        progress.setWindowTitle("PDP Verification");
        progress.setCancelButton(NULL);

        QList<int> List, mList;

        for(int j = 0; j < TOTAL_COVERED_HASHES; j++)
        {
            progress.setValue(j);

            int permutated_index;
            permutated_index = IndexPermutator(session_key, j, d, mList, List); // j =  index
            List.append(permutated_index);

            QByteArray md5 = retrieveMd5(permutated_index + 1, TokenList,
                                          key_secret, folder_id);
            stream.append(md5);
        }

        progress.setValue(TOTAL_COVERED_HASHES);
    }

    EngineCryptographicHashing hashing;
    QByteArray sha1 = hashing.calculateSha1(stream);

    // Retrieve tokens from database setting:
    QStringList tokens = dbcontr_s.retrievePDPtokens();

    if(tokens.at(0) == sha1.toHex())
        return true;
    else
        return false;
}

bool EnginePDP::getIsChanged()
{
    return isChanged;
}

QStringList *EnginePDP::getDecodingTokenList()
{
    return DecodingTokenList;
}


