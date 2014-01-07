#ifndef DATABASECONTROLLERENGINES_H
#define DATABASECONTROLLERENGINES_H

#include <QStringList>

// QtSql
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlRecord>

#include <QDebug>

class DatabaseControllerEngines
{
public:
    DatabaseControllerEngines();
    ~DatabaseControllerEngines();

    bool checkMD5_LOG();

    void createMD5_LOG();

    void insertInfo(QString md5, int size, int chunk_size,
                    QStringList InfoList, int counter);

    void insertInfo(QMap<QString, QList<int> > metadata,
                    QMap<QString, QStringList> Sources,
                    QMap<QString, int> mcounter);

    void updateTable(QString md5, int coordinate, QStringList InfoList);

    QStringList retrieveInfo(QString md5);

    QStringList retrieveInfo(int index);

    int getSize(QString md5);

    int getSize(int index);

    int getChunkSize(int index);

    int getCounter(QString md5);

    void updateCounter(int counter, QString md5);

    void updateCounter(QMap<QString, int> mcounter);

    QStringList getMD5List();

    QString getMD5(int id);

    int getTotalChunks();

    void deleteEntry(QString md5);

    void deleteEntry(QStringList MD5);

    void recreateMD5_LOG();

    void reorganizeDatabase();
};

#endif // DATABASECONTROLLERENGINES_H
