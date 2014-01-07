#ifndef ENGINECODING_H
#define ENGINECODING_H

#include <vector>
#include <fstream>

#include <QStringList>
#include <QProgressDialog>
#include <QDebug>

#include "DBContr/databasecontrollerengines.h"
#include "CoreEngines/enginecryptographichashing.h"
#include "NetworkContr/networkcontrollerparallel.h"

using namespace std;

class EngineCoding
{
    unsigned short *gflog, *gfilog;

    unsigned int nw(int w);
    unsigned int nwm1(int w);

    int setupTables(int w);
    int mult(int a, int b, int w);
    int div(int a, int b, int w);

    unsigned int *createInterpolationPoints(int k, int n, int w);
    unsigned short *createHsrcMatrix(int k, int n, int w);
    unsigned short *createHsrcMatrixTranspose(int k, int n, int w);

public:
    EngineCoding();
    ~EngineCoding();

    // for encoding:
    void encodeChunks(unsigned char *memblock, int chunk_size);
    // QStringList* uploadChunks(QString md5, QStringList *TokenList);

    // for decoding:
    void createAugmentedMatrix(int size, QStringList InfoList,
                               QStringList *TokenList,
                               QStringList key_secret,
                               QStringList folder_id);
    void decodeChunks(int size);

    // Use (n=7,k=3)-HSRC with the underlying field GF(2^8)
    static const int n = 7, k = 3, w = 8;

    bool TokenIsChanged;
    QStringList *DecodingTokenList;

    struct encoding_info
    {
        int size;
        unsigned char *encoded_vector;
    }en_info;

    struct decoding_info
    {
        unsigned char *augmented_matrix;
        unsigned char *recovery_vector;
        unsigned char *gluing_vector;
    }de_info;

    struct repairing_info
    {
        int file_size;
        unsigned char *memblock1;
        unsigned char *memblock2;
        unsigned char *repaired_block;
        QByteArray *repaired_streams;
    }re_info;
};

#endif // ENGINECODING_H
