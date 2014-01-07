#ifndef DATABASECONTROLLERMETADATA_H
#define DATABASECONTROLLERMETADATA_H

#include <QStringList>

// QtSql
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlRecord>

#include <QDebug>

class DatabaseControllerMetadata
{
public:
    DatabaseControllerMetadata();
    ~DatabaseControllerMetadata();

    // create table metadata:
    QList<QVariant> insertEntries(QString title, qint64 size, QString format,
                                 QString location);

    QString getTitle(int file_id);

    void deleteEntries(QList<int> file_idList);

    QList<int> returnMetaInfo(int file_id);

    void deleteEntry(int file_id);

    // create table file_id:
    void createDescription(QString tablename);

    void insertDescription(QString tablename, int i, QString md5,
                           int offset, int chunk_size);

    void insertDescription(QString tablename,
                           QMap<int, QVariantList> Description);

    QVariantList* returnDescription(int file_id);

    int getNoChunks(int file_id);

    void deleteTable(int file_id);

    void reorganizeDatabase();
};

#endif // DATABASECONTROLLERMETADATA_H
