#ifndef ENGINEDELETE_H
#define ENGINEDELETE_H

#include <QStringList>
#include <QProgressDialog>
#include <QFile>

#include "DBContr/databasecontrollerengines.h"
#include "DBContr/databasecontrollermetadata.h"
#include "NetworkContr/networkcontrollerparallel.h"
#include "CoreEngines/enginepdp.h"

class EngineDelete
{
    bool isChanged;
    QStringList *EncodingTokenList;

public:
    EngineDelete();

    void deleteRecord(int file_id, QStringList *TokenList);

    // for tokens:
    bool getIsChanged();
    QStringList* getTokenList();
};

#endif // ENGINEDELETE_H
