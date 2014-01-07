#include "CoreEngines/enginecoding.h"

EngineCoding::EngineCoding()
{
    TokenIsChanged = false;
    setupTables(w);
}

EngineCoding::~EngineCoding()
{
    delete[] gflog;
    delete[] gfilog;
}

unsigned int EngineCoding::nw(int w)
{
    unsigned int result;
    result = (1 << w);
    return result;
}

unsigned int EngineCoding::nwm1(int w)
{
    unsigned int result;
    result = (1 << w) - 1;
    return result;
}

int EngineCoding::setupTables(int w)
{
    unsigned int b, log, prim_poly;

    switch(w)
    {
        case 1:  prim_poly =           03; break;
        case 2:  prim_poly =           07; break;
        case 3:  prim_poly =          013; break;
        case 4:  prim_poly =          023; break;
        case 5:  prim_poly =          045; break;
        case 6:  prim_poly =         0103; break;
        case 7:  prim_poly =         0211; break;
        case 8:  prim_poly =         0435; break;
        case 9:  prim_poly =        01021; break;
        case 10: prim_poly =        02011; break;
        case 11: prim_poly =        04005; break;
        case 12: prim_poly =       010123; break;
        case 13: prim_poly =       020033; break;
        case 14: prim_poly =       042103; break;
        case 15: prim_poly =      0100003; break;
        case 16: prim_poly =      0210013; break;
        default: return -1;
    }

    gflog = new unsigned short[nw(w)]; // maybe occupy very large memory!!
    gfilog = new unsigned short[nw(w)];

    b = 1;
    for(log = 0; log < nwm1(w); log++)
    {
        gflog[b] = (unsigned short) log;
        gfilog[log] = (unsigned short) b;
        b = b << 1;
        if(b & nw(w))
        {
            b = b ^ prim_poly;
        }
    }

    return 0;
}

int EngineCoding::mult(int a, int b, int w)
{
    int sum_log;

    if (a == 0 || b == 0) return 0;
    sum_log = gflog[a] + gflog[b];
    if (sum_log >= nwm1(w)) sum_log -= (nwm1(w));
    return gfilog[sum_log];
}

int EngineCoding::div(int a, int b, int w)
{
    int diff_log;

    if(a == 0) return 0;
    if(b == 0) return -1;  /*cannot divide by zero*/
    diff_log = gflog[a] - gflog[b];
    if(diff_log < 0) diff_log += (nwm1(w));
    return gfilog[diff_log];
}

int compare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

unsigned int *EngineCoding::createInterpolationPoints(int k, int n, int w)
{
    unsigned int *interpolation_points;
    interpolation_points = new unsigned int[n];

    // testing for the parameters k, n:
    if(n <= k)
    {
         qDebug() << "ERROR! The interpolation points cannot be generated!";
         exit(1);
    }
    int counter = 0;
    for(int i = 1; i < w + 1; i++)
    {
        if(n == nwm1(i))
        {
            counter ++;
        }
        else continue;
    }

    // creating interpolation points:
    if(counter == 1)
    {
        for(int i = 0; i < n; i++)
        {
            interpolation_points[i] = gflog[i + 1];
        }

        qsort(interpolation_points, n, sizeof(int), compare);

        return interpolation_points;
    }
    else
    {
        qDebug() << "ERROR! The interpolation points cannot be generated!";
        exit(1);
    }
}

unsigned short *EngineCoding::createHsrcMatrix(int k, int n, int w)
{
    unsigned short *h_matrix;
    unsigned int *interpolation_points;
    h_matrix = new unsigned short[k*n];
    interpolation_points = new unsigned int[n];

    for(int i = 0; i < n; i++)
    interpolation_points[i] = createInterpolationPoints(k, n, w)[i];

    // creating a hsrc matrix with parameters k and n:
    for(int i = 0; i < k; i++)
    {
        for(int j = 0; j < n; j++)
        {
            h_matrix[i*n + j] = gfilog[nw(i)*interpolation_points[j]%nwm1(w)];
        }
    }

    delete[] interpolation_points;

    return h_matrix;
}

unsigned short *EngineCoding::createHsrcMatrixTranspose(int k, int n, int w)
{
    unsigned short *h_matrix;
    unsigned int *interpolation_points;
    h_matrix = new unsigned short[k*n];
    interpolation_points = new unsigned int[n];

    for(int i = 0; i < n; i++)
    interpolation_points[i] = createInterpolationPoints(k, n, w)[i];

    // creating a hsrc matrix transpose with parameters k and n:
    for(int j = 0; j < n; j++)
    {
        for(int i = 0; i < k; i++)
        {
            h_matrix[j*k + i] = gfilog[nw(i)*interpolation_points[j]%nwm1(w)];
        }
    }

    delete[] interpolation_points;

    return h_matrix;
}

void EngineCoding::encodeChunks(unsigned char *memblock, int chunk_size)
{
    unsigned char *input_matrix;

    // Check whether the chunk size is a multiple of k = 3.
    if(chunk_size%k != 0) en_info.size = chunk_size + (k - chunk_size%k);
    else en_info.size = chunk_size;

    // Use zero padding scheme.
    input_matrix = new unsigned char[en_info.size];
    for(int i = 0; i < en_info.size; i++)
    {
        if(i < chunk_size) input_matrix[i] = memblock[i];
        else input_matrix[i] = 0;
    }

    // Begin HSRC encoding.
    unsigned short *h_matrix;
    h_matrix = new unsigned short[k*n];

    for(int i = 0; i < k*n; i++)
        h_matrix[i] = createHsrcMatrixTranspose(k, n, w)[i];

    en_info.encoded_vector = new unsigned char[(en_info.size*n/k)];

    for(int i = 0; i < (en_info.size*n/k); i++)
    {
        en_info.encoded_vector[i] = 0;
    }

    for(int i = 0; i < (en_info.size/k); i++)
    {
        for(int j = 0; j < n; j++)
        {
            for(int l = 0; l < k; l++)
            {
                en_info.encoded_vector[i*n + j] ^= mult(input_matrix[i*k + l], h_matrix[j*k + l], w);
            }
        }
    }

    delete[] input_matrix;
    delete[] h_matrix;
}

/*
QStringList *EngineCoding::uploadChunks(QString md5, QStringList *TokenList)
{
    QStringList *InfoList;
    InfoList = new QStringList[7];

    unsigned char **encoded_parts;
    encoded_parts = new unsigned char*[n];

    for(int i = 0; i < n; i++)
        encoded_parts[i] = new unsigned char[en_info.size/k];

    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < (en_info.size/k); j++)
            encoded_parts[i][j] = en_info.encoded_vector[j*n + i];

        if(i < n - 1)
        {
            // upload encoded chunks.
            QStringList List;
            QString chunk_name = "";
            chunk_name.append(md5);
            chunk_name.append(QString(".encoded%1").arg(i + 1));
            chunk_name.append(".bin");

            // Mask the chunk_name by Md4.
            QString md4;
            EngineCryptographicHashing hashing;
            md4 = hashing.calculateHash(chunk_name);

            NetworkController controller;
            List = controller.upload(md4, en_info.size/k, i, encoded_parts[i], TokenList);

            InfoList[i] = List;
        }
        else
        {
            if(!QDir("local").exists())
            {
                QDir().mkpath("local");
            }

            // store encoded blocks as local copies:
            QStringList List;
            QString md;
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
                for(int l = 0; l < en_info.size/k; l++)
                    upload_stream.append(encoded_parts[n - 1][l]);

                file.write(upload_stream);

                file.flush();
                file.close();

                md = "";
                List.append("./local/" + md4);
                List.append(md);
                InfoList[n - 1] = List;
            }
        }
    }

    for(int i = 0; i < n; i++)
        delete[] encoded_parts[i];

    delete encoded_parts;
    delete[] en_info.encoded_vector;

    return InfoList;
}
*/

// for decoding/PDP:
void EngineCoding::createAugmentedMatrix(int size, QStringList InfoList,
                                         QStringList *TokenList,
                                         QStringList key_secret,
                                         QStringList folder_id)
{
    unsigned short *h_matrix;
    unsigned int *coordinates;

    QByteArray *download_streams;

    // download pieces:
    NetworkControllerParallel controller(key_secret, folder_id);
    controller.download(InfoList, TokenList);

    QEventLoop eventLoop;
    QObject::connect(&controller, SIGNAL(completed()), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    download_streams = controller.download_streams;

    if(controller.TokenIsChanged)
    {
        TokenIsChanged = true;
        DecodingTokenList = controller.getDecodingTokenList();
    }
    else
    {
        DecodingTokenList = TokenList;
    }

    // creating augmented matrixT:
    de_info.augmented_matrix = new unsigned char[size*(k + 1)];
    coordinates = new unsigned int[k];

    for(int i = 0; i < k; i++)
    {
        if(i < k - 1)
        {
            // retrieve the coordinates of the corresponding encoded files:
            coordinates[i] = download_streams[i][download_streams[i].length() - 1] - '0';

            int counter = 0;
            for(int m = 0; m < size*(k + 1); m += k*(k + 1))
            {
                de_info.augmented_matrix[m + i*(k + 1) + k] = download_streams[i][counter];
                counter++;
            }
        }
        else
        {
            // retrieve the coordinates of the corresponding encoded files:
            coordinates[k - 1] = n - 1;

            char *memblock;
            memblock = new char[size/k];

            QFile file(InfoList[6]);
            if(!file.open(QIODevice::ReadOnly))
            {
                qDebug() << QString("Unable to open file!");
            }
            else
            {
                QDataStream in(&file);
                int success = in.readRawData(memblock, size/k);
                file.close();

                if(success > -1)
                {
                    int counter = 0;
                    for(int m = 0; m < size*(k + 1); m += k*(k + 1))
                    {
                        de_info.augmented_matrix[m + i*(k + 1) + k] = memblock[counter];
                        counter++;
                    }
                }
            }

            delete[] memblock;
        }
    }

    h_matrix = new unsigned short[k*n];
    for(int i = 0; i < k*n; i++)
        h_matrix[i] = createHsrcMatrixTranspose(k, n, w)[i];

    for(int m = 0; m < size*(k + 1); m += k*(k+1))
    {
        for(int j = 0; j < k; j++)
        {
            for(int i = 0; i < k; i++)
            {
                de_info.augmented_matrix[m + j*(k + 1) + i] = h_matrix[coordinates[j]*k + i];
            }
        }
    }

    delete[] h_matrix;
    delete[] coordinates;
    delete[] download_streams;
}

void EngineCoding::decodeChunks(int size)
{
    // Gaussian Elimination (the number of rows >= 2^(k-1)+1):
    for(int m = 0; m < size*(k + 1); m += k*(k+1))
    {
    for(int j = 0; j < k - 1; j++)
    {
        unsigned char *multiples;
        multiples = new unsigned char[k - (j + 1)];

        for(int i = j + 1; i < k; i++)
        {
            unsigned char *product;
            product = new unsigned char[k + 1 - j];

            // compute multiples:
            multiples[i - (j + 1)] = div(de_info.augmented_matrix[m + i*(k + 1) + j], de_info.augmented_matrix[m + j*(k + 1) + j], w);

            for(int l = j; l < k + 1; l++)
            {
                product[l - j] = mult(multiples[i - (j + 1)], de_info.augmented_matrix[m + j*(k + 1) + l], w);

                //  xor operation:
                de_info.augmented_matrix[m + i*(k + 1) + l] ^= product[l - j];
            }

            delete[] product;
        }

        delete[] multiples;
    }
    }

    // Back Substitution:
    unsigned char *sum;
    sum = new unsigned char[k];
    de_info.recovery_vector = new unsigned char[size];
    int counter2 = 0;

    for(int m = 0; m < size*(k + 1); m += k*(k + 1))
    {
    for(int i = k - 1; i >= 0; i--)
    {
        sum[i] = 0;

        if (i == k - 1) sum[i] = 0;
        else if (i != k - 1)
             for(int j = k - 1; j >= i + 1 ; j--)
             {
                 sum[i] ^= mult(de_info.augmented_matrix[m + i*(k + 1) + j], de_info.recovery_vector[(counter2)*k + j] , w);
             }

        de_info.recovery_vector[(counter2)*k + i] = div((de_info.augmented_matrix[m + i*(k + 1) + k] ^ sum[i]),
           de_info.augmented_matrix[m + i*(k + 1) + i], w);
    }
    counter2++;
    }

    delete[] sum;
    delete[] de_info.augmented_matrix;
}
