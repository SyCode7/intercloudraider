#include "enginedelete.h"

EngineDelete::EngineDelete()
{
    isChanged = false;
}

void EngineDelete::deleteRecord(int file_id, QStringList *TokenList)
{
    // retrieve the info:
    QList<QVariant> *description;
    int no_chunks;
    QString title;
    DatabaseControllerMetadata dbcontr_m;
    description = dbcontr_m.returnDescription(file_id);
    no_chunks = dbcontr_m.getNoChunks(file_id);
    title = dbcontr_m.getTitle(file_id);

    QStringList key_secret, folder_id;
    DatabaseControllerSetting dbcontr_s;
    key_secret = dbcontr_s.retrieveKeySecret();
    if(dbcontr_s.checkBackupFolder())
    {
        folder_id = dbcontr_s.retrieveFolderId();
    }

    // Delete pieces at storage providers and local disk.
    QProgressDialog progress(QString("Deleting: entry %1").arg(file_id),
                             "Cancel", 0, no_chunks);
    progress.setWindowFlags(Qt::WindowTitleHint);
    progress.setWindowTitle("Deleting");
    progress.setCancelButton(NULL);

    DatabaseControllerEngines dbcontr_e;
    QMap<QString, int> mcounter;
    QStringList MD5;

    for(int i = 0; i < no_chunks; i++)
    {
        progress.setValue(i);

        QString md5 = (description[i]).at(0).toString();

        int counter;
        counter = dbcontr_e.getCounter(md5);

        if(counter > 0)
        {
            counter--;
            mcounter[QString("%1").arg(md5)] = counter;
            // dbcontr_e.updateCounter(counter, md5);
        }

        if(counter == 0) // If the counter is 0, remove the encoded pieces of the chunk.
        {
            QStringList InfoList = dbcontr_e.retrieveInfo(md5);

            // Remove remote pieces:
            NetworkControllerParallel nwcontroller(key_secret, folder_id);
            nwcontroller.drop(InfoList, TokenList);

            QEventLoop eventLoop;
            QObject::connect(&nwcontroller, SIGNAL(completed()), &eventLoop, SLOT(quit()));
            eventLoop.exec();

            if(nwcontroller.TokenIsChanged)
            {
                isChanged = true;
                EncodingTokenList = nwcontroller.getTokenList();
                TokenList = EncodingTokenList;
            }

            // Remove local pieces:
            QFile::remove(InfoList[6]);

            // Append md5 to qstringlist MD5:
            MD5.append(md5);
        }
    }

    dbcontr_e.updateCounter(mcounter);
    dbcontr_e.deleteEntry(MD5); // Remove the entry in MD5_LOG
    dbcontr_e.reorganizeDatabase();

    if(dbcontr_s.checkPDPtokens() && !MD5.isEmpty())
    {
        // Rearrange id:
        dbcontr_e.recreateMD5_LOG();

        dbcontr_s.deletePDPtokens();
        dbcontr_s.reorganizeDatabase();

        // Regenerate PDP tokens, since the deleted chunks may included in
        // the verification tokens:
        if(dbcontr_e.getTotalChunks() != 0)
        {
            EnginePDP pdp;
            QStringList tokens = pdp.PDPSetup();
            dbcontr_s.createPDPtokens(tokens);
        }
    }

    dbcontr_m.deleteEntry(file_id);
    dbcontr_m.deleteTable(file_id);
    dbcontr_m.reorganizeDatabase();

    qDebug() << QString("%1 deleted!").arg(title);

    progress.setValue(no_chunks);
}

bool EngineDelete::getIsChanged()
{
    return isChanged;
}

QStringList* EngineDelete::getTokenList()
{
    return EncodingTokenList;
}
