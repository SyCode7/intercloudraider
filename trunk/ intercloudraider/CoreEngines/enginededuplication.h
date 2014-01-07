#ifndef ENGINEDEDUPLICATION_H
#define ENGINEDEDUPLICATION_H

#include <QFile>
#include <QDir>
#include <QProgressDialog>

// for RabinFingerprinting64
#include <cstdint>

#include "CoreEngines/enginecoding.h"
#include "CoreEngines/enginecryptographichashing.h"
#include "DBContr/databasecontrollermetadata.h"
#include "DBContr/databasecontrollerengines.h"
#include "NetworkContr/networkcontrollerparallel.h"

class EngineDeduplication
{
    static const int w = 8;

    // Fix an irreducible polynomial of degree 8: x^8 + x^4 + x^3 + x + 1.
    static const unsigned char IRRE_POLY8 = 0x1B;

    // Fix an irreducible polynomial of degree 32: x^32 + x^7 + x^3 + x^2 + 1.
    static const unsigned int IRRE_POLY32 = 0x0000008D;

    // Set the window size to 48bytes.
    static const int w_size = 48;

    // Set the maximum and minimum threholds to be 64KB = 2^16bytes,
    // and 16KB = 2^14bytes respectively:
    static const unsigned int T_MAX = 0xFFFF, T_MIN = 0x3FFF;
    // static const unsigned int T_MAX = 0x7FFF, T_MIN = 0x1FFF; // T_max = 32KB, T_min = 8KB
    // static const unsigned int T_MAX = 0x1FFFF, T_MIN = 0x7FFF; // T_max = 128KB, T_min = 32KB
    // static const unsigned int T_MAX = 0x3FFFF, T_MIN = 0xFFFF; // T_max = 256KB, T_min = 64KB

    // Set the main (and backup divisors) to be 2^14 (and 2^13, half of main),
    // so that the expected chunk size is 32KB:
    static const unsigned int D_MAIN = 0x4000, D_BACKUP = 0x2000;
    // static const unsigned int D_MAIN = 0x2000, D_BACKUP = 0x1000; // D_main = 2^13, D_min = 2^12
                                                                    // average chunk = 16KB
    // static const unsigned int D_MAIN = 0x8000, D_BACKUP = 0x4000; // D_main = 2^15, D_min = 2^14
                                                                           // average chunk = 64KB
    // static const unsigned int D_MAIN = 0x10000, D_BACKUP = 0x8000; // D_main = 2^16, D_min = 2^15
                                                                       // average chunk = 128KB

    // for decoding:
    unsigned char *gluing_vector;
    int file_size;

    bool isChanged;
    QStringList *EncodingTokenList;
    QStringList *DecodingTokenList;

public:
    EngineDeduplication();

    // calculate 8 bits Rabin fingerprint:
    unsigned char RabinFingerprinting8(unsigned char tempRemainder, unsigned char BitsString);
    unsigned char RabinFingerprinting8(const char A[], int length);

    // calculate 32 bits Rabin fingerprint:
    unsigned int RabinFingerprinting32(unsigned int tempRemainder, unsigned int BitsString);
    unsigned int RabinFingerprinting32(const char A[], int length);

    // chunking algorithm with irreducible polynomial of degree 32:
    unsigned int SlidingFunction32(char *memblock, int i);
    QList<int> TTTD32(char *memblock, int file_size);

    // backup algorithm:
    void BackupAlgorithm(QString inputpath, int file_id, QStringList *TokenList);

    // retrieve algorithm:
    void chunksGluingFunction(int ctr, int chunk_size, unsigned char *recovery_vector);
    void RetrieveAlgorithm(int file_id, QString filename, QString outputpath,
                           QStringList *TokenList);
    void writeFile(QString outputpath, QString filename);

    // for tokens:
    bool getIsChanged();
    QStringList* getEncodingTokenList();
    QStringList* getDecodingTokenList();

};

#endif // ENGINEDEDUPLICATION_H
