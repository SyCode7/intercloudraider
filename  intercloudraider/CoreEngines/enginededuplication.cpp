#include "enginededuplication.h"

EngineDeduplication::EngineDeduplication()
{
    isChanged = false;
}

// Calculate 8 bits Rabin fingerprint of two bytes string of '0' and '1'.
unsigned char EngineDeduplication::RabinFingerprinting8(unsigned char tempRemainder, unsigned char BitsString)
{
    // Suppose that tempRamainder = (a_1 a_2 ... a_8), BitsString = (b_1 b_2 ... b_8).
    unsigned char remainder;

    for(int i = 0; i < 8; i++)
    {
        // Use the formula (a_2 a_3 ... a_8 b_1) + a_1*(c_1 c_2 ... c_8)
        // where IRRE_POLY8 = (c_1 c_2 ... c_8) = (00011011).

        if((tempRemainder & (1 << 7)) != 0)
        {
            if((BitsString & (1 << 7)) != 0)
            {
                // case 1: a_1 = 1, b_1 = 1
                remainder = ((tempRemainder << 1) ^ 1) ^ (IRRE_POLY8);
            }
            else
            {
                // case 2: a_1 = 1, b_1 = 0
                remainder = (tempRemainder << 1) ^ (IRRE_POLY8);
            }
        }
        else
        {
            if((BitsString & (1 << 7)) != 0)
            {
                // case 3: a_1 = 0, b_1 = 1
                remainder = (tempRemainder << 1) ^ 1;
            }
            else
            {
                // case 4: a_1 = 0, b_1 = 0
                remainder = tempRemainder << 1;
            }
        }

        // Shift the bits in BitsString one position to the right.
        BitsString = (BitsString << 1);
        tempRemainder = remainder;
    }

    return remainder;
}

// Calculate 8 bits Rabin fingerprint of 'length' bytes string.
unsigned char EngineDeduplication::RabinFingerprinting8(const char A[], int length)
{
    // Set the byte at offset 0 as the remainder.
    unsigned char remainder = A[0];

    for(int i = 1; i < length; i++)
    {
        // Consume the remaining bytes at offset i for fingerprinting.
        remainder = RabinFingerprinting8(remainder, A[i]);
    }

    // The last remainder is the Rabin fingerprint.
    return remainder;
}

// calculate 32 bits Rabin fingerprint of 8 bytes string of '0' and '1'.
unsigned int EngineDeduplication::RabinFingerprinting32(unsigned int tempRemainder, unsigned int BitsString)
{
    // Suppose that tempRamainder = (a_1 a_2 ... a_32), BitsString = (b_1 b_2 ... b_32).
    unsigned int remainder;

    for(int i = 0; i < 32; i++)
    {
        // Use the formula (a_2 a_3 ... a_32 b_1) + a_1*(c_1 c_2 ... c_32)
        // where IRRE_POLY32 = (c_1 c_2 ... c_32) = (00000000 00000000 00000000 10001101).

        if((tempRemainder & (1 << 31)) != 0)
        {
            if((BitsString & (1 << 31)) != 0)
            {
                // case 1: a_1 = 1, b_1 = 1
                remainder = ((tempRemainder << 1) ^ 1) ^ (IRRE_POLY32);
            }
            else
            {
                // case 2: a_1 = 1, b_1 = 0
                remainder = (tempRemainder << 1) ^ (IRRE_POLY32);
            }
        }
        else
        {
            if((BitsString & (1 << 31)) != 0)
            {
                // case 3: a_1 = 0, b_1 = 1
                remainder = (tempRemainder << 1) ^ 1;
            }
            else
            {
                // case 4: a_1 = 0, b_1 = 0
                remainder = tempRemainder << 1;
            }
        }

        // Shift the bits in BitsString one position to the right.
        BitsString = (BitsString << 1);
        tempRemainder = remainder;
    }

    return remainder;
}

// Calculate 32 bits Rabin fingerprint of 'length' bytes string.
unsigned int EngineDeduplication::RabinFingerprinting32(const char A[], int length)
{
    int size = length/4;
    unsigned int remainder;

    if(length%4 == 0) // if size is a multiple of 4
    {
        // Construct the word sequence by shifting every 4 bytes of A[].
        unsigned int* words = new unsigned int[size];

        for(int i = 0; i < size; i++)
        {
            words[i] = (A[4*i] << 24) ^ (A[4*i + 1] << 16) ^
                       (A[4*i + 2] << 8) ^ A[4*i + 3];
        }

        // Set the first 4 bytes as the remainder.
        remainder = words[0];

        for(int i = 1; i < size; i++)
        {
            // Consume the words for fingerprinting.
            remainder = RabinFingerprinting32(remainder, words[i]);
        }

        delete[] words;
    }
    else // if size is NOT a multiple of 4
    {
        // Construct the word sequence by shifting every 4 bytes of A[].
        unsigned int* words = new unsigned int[size + 1];

        for(int i = 0; i < size; i++)
        {
            words[i] = (A[4*i] << 24) ^ (A[4*i + 1] << 16) ^
                       (A[4*i + 2] << 8) ^ A[4*i + 3];
        }

        if(length%4 == 1)
        {
            words[size] = (A[4*size] << 24);
        }

        if(length%4 == 2)
        {
            words[size] = (A[4*size] << 24) ^ (A[4*size + 1] << 16);
        }

        if(length%4 == 3)
        {
            words[size] = (A[4*size] << 24) ^ (A[4*size + 1] << 16) ^
                          (A[4*size + 2] << 8);
        }

        // Set the first 4 bytes as the remainder.
        remainder = words[0];

        for(int i = 1; i < size + 1; i++)
        {
            // Consume the words for fingerprinting.
            remainder = RabinFingerprinting32(remainder, words[i]);
        }

        delete[] words;
    }

    return remainder;
}

// Calculate 32 bits Rabin fingerprint of 48bytes window.
unsigned int EngineDeduplication::SlidingFunction32(char *memblock, int i)
{
    unsigned int fingerprint;

    char *window = new char[w_size];

    for(int j = 0; j < w_size; j++)
    {
        window[j] = memblock[j + i];
    }

    // Compute the 32 bits Rabin fingerprint of the 48 bytes window.
    fingerprint = RabinFingerprinting32(window, w_size);

    delete[] window;

    return fingerprint;
}

QList<int> EngineDeduplication::TTTD32(char *memblock, int file_size)
{
    // If file_size is no more than T_MIN, there is no breakpoint,
    // the set breakPoints is empty, and the whole file is a chunk.
    QList<int> breakPoints;

    // Initialize the last and backup breakpoints to be 0:
    int l = 0, backupBreak = 0;

    // Execute TTTD.
    for(int p = 0; p < file_size; p++)
    {
        if(p - l < T_MIN)
        {
            // not at minimum size yet
            continue;
        }

        unsigned int fingerprint;
        fingerprint = SlidingFunction32(memblock, p);

        if((fingerprint % D_BACKUP) == 0)
        {
            // possible backup break
            backupBreak = p;
        }

        if((fingerprint % D_MAIN) == 0)
        {
            // We found a breakpoint before T_max.
            // Add to list of breakpoints:
            breakPoints.append(p);

            // Set the last breakpoint to be position p:
            l = p;
            // Reset the backupBreak to 0:
            backupBreak = 0;
            continue;
        }

        if(p - l < T_MAX)
        {
            // We have failed to find a breakpoint, but we are not
            // at the maximum yet.
            continue;
        }

        // When we reach here (p - l = T_MAX), we have not found a
        // breakpoint with D_MAIN.
        // If there is a backupBreak, use it.
        // Otherwise impose a hard threshold.

        if(backupBreak != 0)
        {
            breakPoints.append(backupBreak);

            // Set the last breakpoint to be position backupBreak:
            l = backupBreak;
            // Reset the backupBreak to 0:
            backupBreak = 0;
        }
        else
        {
            breakPoints.append(p);

            // Set the last breakpoint to be position p = l + T_MAX:
            l = p;
            // Reset the backupBreak to 0:
            backupBreak = 0;
        }
    }

    return breakPoints;
}

void EngineDeduplication::BackupAlgorithm(QString inputpath, int file_id, QStringList *TokenList)
{
    // Begin Chunking Algorithm (TTTD):
    QList<int> breakPoints;

    // Read file.
    char *memblock;
    int file_size;

    QFile file(inputpath);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << QString("Unable to open file!");
    }
    else
    {
        file_size = file.size();
        qDebug() << QString("The file size is %1 bytes.").arg(file_size);
        memblock = new char[file_size];
        QDataStream in(&file);
        int success = in.readRawData(memblock, file_size);
        file.close();

        // clock_t start = clock();

        if(success > -1)
        {
            // Use the TTTD32 to determine breakpoints.
            breakPoints = TTTD32(memblock, file_size);
        }

        /* int ChunkingTime = (clock() - start);

        QFile file("ChunkingTime.txt");
        if(!file.open(QIODevice::WriteOnly|QIODevice::Append))
        {
            qDebug() << QString("Unable to open file!");
        }

        else
        {
            QTextStream out(&file);
            out << ChunkingTime << "\n";
            file.flush();
            file.close();
        }*/
    }

    /*int TotalEncode_UploadTime = 0;
    int storage_saving = 0;
    int MetaDataTime = 0;

    clock_t start = clock();*/

    QStringList MD5_List;
    QStringList NewAddedList;
    QMap<QString, int> mcounter;
    QMap<QString, QList<int> > metadata;
    QMap<QString, QStringList> Sources;
    QMap<int, QVariantList> Description;

    QStringList key_secret, folder_id;
    DatabaseControllerSetting dbcontr_s;
    key_secret = dbcontr_s.retrieveKeySecret();
    if(dbcontr_s.checkBackupFolder())
    {
        folder_id = dbcontr_s.retrieveFolderId();
    }

    // Get the md5 from the table md5_log if it exists.
    DatabaseControllerEngines dbcontr_e;
    if(dbcontr_e.checkMD5_LOG())
    {
        MD5_List = dbcontr_e.getMD5List();
    }
    else
    {
        dbcontr_e.createMD5_LOG();
    }

    // Use the breakpoints for indexing and deduplicating:
    int no_chunks = breakPoints.size() + 1;

    QProgressDialog progress(QString("Backup: %1").arg(inputpath),
                             "Cancel", 0, no_chunks);
    progress.setWindowFlags(Qt::WindowTitleHint);
    progress.setWindowTitle("Initiating Backup");
    progress.setCancelButton(NULL);

    for(int i = 0; i < no_chunks; i++)
    {
        progress.setValue(i);

        // Buffer the incoming chunks.
        unsigned char *chunk;
        int chunk_size;   

        if(breakPoints.size() == 0)
        {
            chunk_size = file_size;
            chunk = new unsigned char[chunk_size];

            for(int j = 0; j < chunk_size; j++)
                chunk[j] = memblock[j];
        }
        else
        {
            if(i == 0)
            {
                chunk_size = breakPoints.at(0) + 1;
                chunk = new unsigned char[chunk_size];

                for(int j = 0; j < chunk_size; j++)
                    chunk[j] = memblock[j];
            }
            else if(0 < i && i < no_chunks - 1)
            {
                chunk_size = breakPoints.at(i) - breakPoints.at(i - 1);
                chunk = new unsigned char[chunk_size];

                for(int j = 0; j < chunk_size; j++)
                    chunk[j] = memblock[breakPoints.at(i - 1) + 1 + j];
            }
            else
            {
                chunk_size = file_size - breakPoints.last() - 1;
                chunk = new unsigned char[chunk_size];

                for(int j = 0; j < chunk_size; j++)
                    chunk[j] = memblock[breakPoints.last() + 1 + j];
            }
        }

        // Begin Indexing Algorithm (MD5):
        QByteArray chunk_stream;
        for(int j = 0; j < chunk_size; j++)
            chunk_stream.append(chunk[j]);

        EngineCryptographicHashing hashing;
        QString md5 = hashing.calculateHash(chunk_stream);

        // clock_t start_metadata = clock();

        // Create a file descriptor and store in table [file_id].
        if(i < no_chunks - 1)
        {
            /*dbcontr_m.insertDescription(QString::number(file_id), i + 1, md5,
                                        breakPoints.at(i), chunk_size);*/

            QVariantList entry;
            entry.append(md5);
            entry.append(breakPoints.at(i));
            entry.append(chunk_size);
            Description[i + 1] = entry;
        }
        else
        {
            /*dbcontr_m.insertDescription(QString::number(file_id), i + 1, md5,
                                        file_size - 1, chunk_size);*/
            QVariantList entry;
            entry.append(md5);
            entry.append(file_size - 1);
            entry.append(chunk_size);
            Description[i + 1] = entry;
        }

        // MetaDataTime += (clock() - start_metadata);

        // Begin Deduplication Algorithm (Naive method):
        EngineCoding encoding;

        if(!MD5_List.contains(md5))
        {
            // clock_t start_encode = clock();

            // Start encoding and uploading.
            encoding.encodeChunks(chunk, chunk_size);
            NetworkControllerParallel controller(key_secret, folder_id);
            controller.upload(md5, encoding.n, encoding.k,
                              encoding.en_info.encoded_vector,
                              encoding.en_info.size, TokenList);

            QEventLoop eventLoop;
            QObject::connect(&controller, SIGNAL(completed()), &eventLoop, SLOT(quit()));
            eventLoop.exec();

            if(controller.TokenIsChanged)
            {
                isChanged = true;
                EncodingTokenList = controller.getTokenList();
                TokenList = EncodingTokenList;
            }

            /*TotalEncode_UploadTime += (clock() - start_encode);
            clock_t start_metadata = clock();*/

            // Store MD5 and locations of encoded pieces of chunk
            // at storage providers in the log MD5_LOG.
            /*dbcontr_e.insertInfo(md5, encoding.en_info.size, chunk_size,
                                   controller.InfoList, 1);*/
            QList<int> entry;
            entry.append(encoding.en_info.size);
            entry.append(chunk_size);
            entry.append(1);
            metadata[QString("%1").arg(md5)] = entry;
            Sources[QString("%1").arg(md5)] = controller.InfoList;
            MD5_List.append(md5);
            NewAddedList.append(md5);

            // MetaDataTime += (clock() - start_metadata);
            // storage_saving += chunk_size;
        }
        else
        {
            // clock_t start_metadata = clock();

            // If the new coming chunk is a duplicate, update the counter
            // of the corresponding entry in MD5_LOG by 1.
            /*int counter;
            counter = dbcontr_e.getCounter(md5);
            counter++;
            dbcontr_e.updateCounter(counter, md5);*/

            if(!NewAddedList.contains(md5))
            {
                int counter;
                counter = dbcontr_e.getCounter(md5);
                counter++;
                mcounter[QString("%1").arg(md5)] = counter;
                NewAddedList.append(md5);
            }
            else
            {
                if(!metadata.contains(md5))
                {
                    int counter = mcounter[QString("%1").arg(md5)];
                    counter++;
                    mcounter[QString("%1").arg(md5)] = counter;
                }
                else
                {
                    QList<int> entry = metadata[QString("%1").arg(md5)];
                    entry[2] = entry[2] + 1;
                    metadata[QString("%1").arg(md5)] = entry;
                }
            }

            // MetaDataTime += (clock() - start_metadata);
        }

        delete[] chunk;
    }

    delete[] memblock;

    // To avoid the deadlock situation in sqlite3, info is updated after for loop.
    dbcontr_e.insertInfo(metadata, Sources, mcounter);

    // Create description table for the deduplicating file:
    DatabaseControllerMetadata dbcontr_m;
    dbcontr_m.createDescription(QString::number(file_id));
    dbcontr_m.insertDescription(QString::number(file_id), Description);

    /*
    int DeduplicatingTime = (clock() - start) - TotalEncode_UploadTime - MetaDataTime;

    QFile file1("Deduplicating+IndexingTime.txt");
    if(!file1.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        qDebug() << QString("Unable to open file!");
    }
    else
    {
        QTextStream out(&file1);
        out << DeduplicatingTime << "\n";
        file1.flush();
        file1.close();
    }

    QFile file2("StorageInSaving.txt");

    if(!file2.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        qDebug() << QString("Unable to open file!");
    }
    else
    {
        QTextStream out(&file2);
        out << storage_saving << "\n";
        file2.flush();
        file2.close();
    }

    QFile file3("MetadataTime.txt");
    if(!file3.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        qDebug() << QString("Unable to open file!");
    }
    else
    {
        QTextStream out(&file3);
        out << MetaDataTime << "\n";
        file3.flush();
        file3.close();
    }

    QFile file4("Encode_Upload.txt");
    if(!file4.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        qDebug() << QString("Unable to open file!");
    }
    else
    {
        QTextStream out(&file4);
        out << (TotalEncode_UploadTime / CLOCKS_PER_SEC) << "\n";
        file4.flush();
        file4.close();
    }
    */

    qDebug() << QString("%1: Backup success!").arg(inputpath);
    progress.setValue(no_chunks);
}

void EngineDeduplication::chunksGluingFunction(int ctr, int chunk_size,
                                               unsigned char *recovery_vector)
{
    for(int i = 0; i < chunk_size; i++)
    {
        gluing_vector[ctr + i] = recovery_vector[i];
    }
}

void EngineDeduplication::RetrieveAlgorithm(int file_id, QString filename, QString outputpath,
                                            QStringList *TokenList)
{
    // retrieve the info of the file to be decoded:
    QVariantList*Description;
    QList<int> metaInfo;
    int no_chunks; 

    QStringList key_secret, folder_id;
    DatabaseControllerSetting dbcontr_s;
    key_secret = dbcontr_s.retrieveKeySecret();
    if(dbcontr_s.checkBackupFolder())
    {
        folder_id = dbcontr_s.retrieveFolderId();
    }

    DatabaseControllerEngines dbcontr_e;
    DatabaseControllerMetadata dbcontr_m;
    Description = dbcontr_m.returnDescription(file_id);
    metaInfo = dbcontr_m.returnMetaInfo(file_id);

    file_size = metaInfo[0];
    no_chunks = metaInfo[1];

    QProgressDialog progress(QString("Retrieve: %1").arg(filename),
                             "Cancel", 0, no_chunks);
    progress.setWindowFlags(Qt::WindowTitleHint);
    progress.setWindowTitle("Initiating Retrieve");
    progress.setCancelButton(NULL);

    // set up Galois field Table:
    EngineCoding decoding;
    gluing_vector = new unsigned char[file_size];

    QStringList sources;
    for(int i = 0; i < no_chunks; i++)
    {
        progress.setValue(i);

        QString md5 = (Description[i]).at(0).toString();
        int size;
        size = dbcontr_e.getSize(md5);
        sources = dbcontr_e.retrieveInfo(md5);

        decoding.createAugmentedMatrix(size, sources, TokenList,
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

        int offset, chunk_size, ctr;

        if(i == 0)
        {
            ctr = 0;
            chunk_size = Description[i].at(2).toInt();
        }

        else if(0 < i && i < no_chunks)
        {
            offset = Description[i - 1].at(1).toInt();
            ctr = offset + 1;
            chunk_size = Description[i].at(2).toInt();
        }

        else
        {
            qDebug() << "ERROR!";
        }

        chunksGluingFunction(ctr, chunk_size, decoding.de_info.recovery_vector);
        delete[] decoding.de_info.recovery_vector;

        sources.clear();
    }

    delete[] Description;

    writeFile(outputpath, filename);

    progress.setValue(no_chunks);
}

void EngineDeduplication::writeFile(QString outputpath, QString filename)
{
    // write file:
    QByteArray stream;
    for(int i = 0; i < file_size; i++)
        stream.append(gluing_vector[i]);

    delete[] gluing_vector;

    QFile file(outputpath + "/" + filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << QString("Unable to open file!");
    }
    else
    {
        file.write(stream);

        file.flush();
        file.close();

        qDebug() << QString("%1: Retrieve success!").arg(filename);
    }
}

bool EngineDeduplication::getIsChanged()
{
    return isChanged;
}

QStringList* EngineDeduplication::getEncodingTokenList()
{
    return EncodingTokenList;
}

QStringList* EngineDeduplication::getDecodingTokenList()
{
    return DecodingTokenList;
}














