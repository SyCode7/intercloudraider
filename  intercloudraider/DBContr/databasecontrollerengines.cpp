#include "databasecontrollerengines.h"

DatabaseControllerEngines::DatabaseControllerEngines()
{
}

DatabaseControllerEngines::~DatabaseControllerEngines()
{
}

// Check whether the table MD5_LOG exists.
bool DatabaseControllerEngines::checkMD5_LOG()
{
    bool result;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QStringList tablelist;
            tablelist = db.tables(QSql::Tables);

            if(tablelist.contains("MD5_LOG"))
            {
                db.close();
                result = true;
            }
            else
            {
                db.close();
                result = false;
            }
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            result = checkMD5_LOG();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return result;
}

// Create the table MD5_LOG and insert md5.
void DatabaseControllerEngines::createMD5_LOG()
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QString createString = QString("CREATE TABLE IF NOT EXISTS MD5_LOG");
            createString.append("(id INTEGER PRIMARY KEY, ");
            createString.append("MD5 TEXT NOT NULL, ");
            createString.append("size INTEGER NOT NULL, ");
            createString.append("chunk_size INTEGER NOT NULL, ");

            for(int i = 1; i <= 7; i++)
                createString.append(QString("Id_piece%1 TEXT NULL, ")
                                    .arg(i));

            createString.append("counter INTEGER NOT NULL)");

            QSqlQuery query;
            if (!query.exec(createString))
            {
                qDebug() <<  "Error = " << query.lastError();
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }

    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void DatabaseControllerEngines::insertInfo(QString md5, int size, int chunk_size,
                                           QStringList InfoList, int counter)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QString insertString = QString("INSERT INTO MD5_LOG");
            insertString.append("(id, ");
            insertString.append("MD5, ");
            insertString.append("size, ");
            insertString.append("chunk_size, ");

            for(int i = 1; i <= 7; i++)
                insertString.append(QString("Id_piece%1, ")
                                    .arg(i));

            insertString.append("counter)");

            insertString.append("VALUES(:id, ");
            insertString.append(":MD5, ");
            insertString.append(":size, ");
            insertString.append(":chunk_size, ");

            for(int i = 1; i <= 7; i++)
                insertString.append(QString(":Id_piece%1, ")
                                    .arg(i));

            insertString.append(":counter)");

            QSqlQuery query;
            query.prepare(insertString);
            query.bindValue(":MD5", md5);
            query.bindValue(":size", size);
            query.bindValue(":chunk_size", chunk_size);

            for(int i = 1; i <= 7; i++)
                query.bindValue(QString(":Id_piece%1").arg(i), InfoList[i - 1]); // id

            query.bindValue(":counter", counter);

            if (!query.exec())
            {
                qDebug() <<  "Error = " << query.lastError();
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void DatabaseControllerEngines::insertInfo(QMap<QString, QList<int> > metadata,
                                           QMap<QString, QStringList> Sources,
                                           QMap<QString, int> mcounter)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QMap<QString, QList<int> >::const_iterator j = metadata.constBegin();
            while(j != metadata.constEnd())
            {
                QList<int> entry = j.value();

                QString insertString = QString("INSERT INTO MD5_LOG");
                insertString.append("(id, ");
                insertString.append("MD5, ");
                insertString.append("size, ");
                insertString.append("chunk_size, ");
                insertString.append("counter)");

                insertString.append("VALUES(:id, ");
                insertString.append(":MD5, ");
                insertString.append(":size, ");
                insertString.append(":chunk_size, ");
                insertString.append(":counter)");

                QSqlQuery query;
                query.prepare(insertString);
                query.bindValue(":MD5", j.key());
                query.bindValue(":size", entry[0]);
                query.bindValue(":chunk_size", entry[1]);
                query.bindValue(":counter", entry[2]);

                if (!query.exec())
                {
                    qDebug() <<  "Error = " << query.lastError();
                }

                query.finish();

                j++;
            }

            QMap<QString, QStringList>::const_iterator k = Sources.constBegin();
            while(k != Sources.constEnd())
            {
                QStringList InfoList = k.value();

                QString updateString = QString("UPDATE MD5_LOG SET ");

                for(int i = 1; i < 7; i++)
                    updateString.append(QString("Id_piece%1 =  \'%2\', ")
                    .arg(i).arg(InfoList[i - 1]));

                updateString.append(QString("Id_piece%1 =  \'%2\' ")
                .arg(7).arg(InfoList[6]));
                updateString.append(QString("WHERE MD5 = \'%1\'").arg(k.key()));

                QSqlQuery query;
                query.prepare(updateString);

                if (!query.exec())
                {
                    qDebug() <<  "Error = " << query.lastError();
                }

                query.finish();

                k++;
            }

            QMap<QString, int>::const_iterator i = mcounter.constBegin();
            while (i != mcounter.constEnd())
            {
                QString updateString
                = QString("UPDATE MD5_LOG SET counter = %1 WHERE MD5 = \'%2\'")
                  .arg(i.value()).arg(i.key());

                QSqlQuery query;
                query.prepare(updateString);

                if (!query.exec())
                {
                    qDebug() <<  "Error = " << query.lastError();
                }

                query.finish();

                i++;
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void DatabaseControllerEngines::updateTable(QString md5, int coordinate,
                                            QStringList InfoList)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QString updateString =
                    QString("UPDATE MD5_LOG SET Id_piece%1 = \'%2\' WHERE MD5 = \'%3\'")
                    .arg(coordinate).arg(InfoList[0]).arg(md5);

            QSqlQuery query;
            query.prepare(updateString);

            if (!query.exec())
            {
                qDebug() <<  "Error = " << query.lastError();
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

QStringList DatabaseControllerEngines::retrieveInfo(QString md5)
{
    QStringList sources;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QString selectString = QString("SELECT * FROM MD5_LOG WHERE MD5 = \'%1\'")
                                   .arg(md5);

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);

            for(int i = 4; i < model->columnCount() - 1; i++)
            {
                sources.append(model->record(0).value(i).toString());
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            sources = retrieveInfo(md5);
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return sources;
}

QStringList DatabaseControllerEngines::retrieveInfo(int index)
{
    QStringList InfoList;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QString selectString = QString("SELECT * FROM MD5_LOG WHERE id = %1")
                                   .arg(index);

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);

            for(int i = 4; i < model->columnCount() - 1; i++)
            {
                InfoList.append(model->record(0).value(i).toString());
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            InfoList = retrieveInfo(index);
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return InfoList;
}

int DatabaseControllerEngines::getSize(QString md5)
{
    int size;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QString selectString = QString("SELECT size FROM MD5_LOG WHERE MD5 = \'%1\'")
                                   .arg(md5);

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);

            size = model->record(0).value(0).toInt();

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            size = getSize(md5);
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return size;
}

int DatabaseControllerEngines::getSize(int index)
{
    int size;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QString selectString = QString("SELECT size FROM MD5_LOG WHERE id = %1")
                                   .arg(index);

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);

            size = model->record(0).value(0).toInt();

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            size = getSize(index);
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return size;
}

int DatabaseControllerEngines::getChunkSize(int index)
{
    int chunk_size;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QString selectString = QString("SELECT chunk_size FROM MD5_LOG WHERE id = %1")
                                   .arg(index);

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);

            chunk_size = model->record(0).value(0).toInt();

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            chunk_size = getChunkSize(index);
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return chunk_size;
}

int DatabaseControllerEngines::getCounter(QString md5)
{
    int counter;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QString selectString = QString("SELECT counter FROM MD5_LOG WHERE MD5 = \'%1\'")
                                   .arg(md5);

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);

            counter = model->record(0).value(0).toInt();

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            counter = getCounter(md5);
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return counter;
}

void DatabaseControllerEngines::updateCounter(int counter, QString md5)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QString updateString
            = QString("UPDATE MD5_LOG SET counter = %1 WHERE MD5 = \'%2\'")
              .arg(counter).arg(md5);

            QSqlQuery query;
            query.prepare(updateString);

            if (!query.exec())
            {
                qDebug() <<  "Error = " << query.lastError();
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void DatabaseControllerEngines::updateCounter(QMap<QString, int> mcounter)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QMap<QString, int>::const_iterator i = mcounter.constBegin();
            while (i != mcounter.constEnd())
            {
                QString updateString
                = QString("UPDATE MD5_LOG SET counter = %1 WHERE MD5 = \'%2\'")
                  .arg(i.value()).arg(i.key());

                QSqlQuery query;
                query.prepare(updateString);

                if (!query.exec())
                {
                    qDebug() <<  "Error = " << query.lastError();
                }

                query.finish();

                i++;
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

// Get list of md5 from MD5_LOG.
QStringList DatabaseControllerEngines::getMD5List()
{
    QStringList MD5_List;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QString selectString = QString("SELECT MD5 FROM MD5_LOG");

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);
            while (model->canFetchMore())
                model->fetchMore();

            for(int i = 0; i < model->rowCount(); i++)
            {
                MD5_List.append(model->record(i).value(0).toString());
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            MD5_List = getMD5List();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return MD5_List;
}

// Get md5 from MD5_LOG.
QString DatabaseControllerEngines::getMD5(int id)
{
    QString md5;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QString selectString = QString("SELECT MD5 FROM MD5_LOG WHERE id = %1")
                                   .arg(id);

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);

            md5 = model->record(0).value(0).toString();

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            md5  = getMD5(id);
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return md5;
}

int DatabaseControllerEngines::getTotalChunks()
{
    int total_chunks;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QString selectString = QString("SELECT MD5 FROM MD5_LOG");

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);
            while (model->canFetchMore())
                model->fetchMore();

            total_chunks = model->rowCount();

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            total_chunks = getTotalChunks();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return total_chunks;
}

// Remove one entry from the table MD5_LOG.
void DatabaseControllerEngines::deleteEntry(QString md5)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QString deleteString = QString("DELETE FROM MD5_LOG WHERE MD5 = \'%1\'")
                    .arg(md5);

            QSqlQuery query;
            if (!query.exec(deleteString))
            {
                qDebug() <<  "Error = " << query.lastError();
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

// Remove entries from the table MD5_LOG.
void DatabaseControllerEngines::deleteEntry(QStringList MD5)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            for(int i = 0; i < MD5.size(); i++)
            {
                QString deleteString = QString("DELETE FROM MD5_LOG WHERE MD5 = \'%1\'")
                        .arg(MD5[i]);

                QSqlQuery query;
                if (!query.exec(deleteString))
                {
                    qDebug() <<  "Error = " << query.lastError();
                }
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

void DatabaseControllerEngines::recreateMD5_LOG()
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QString selectString = QString("SELECT * FROM MD5_LOG");

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);
            while (model->canFetchMore())
                model->fetchMore();

            QString dropString = QString("DROP TABLE IF EXISTS MD5_LOG");
            QSqlQuery query;
            if (!query.exec(dropString))
            {
                qDebug() <<  "Error = " << query.lastError();
            }
            else
            {
                qDebug() << QString("The table MD5_LOG is removed");

                QString createString = QString("CREATE TABLE IF NOT EXISTS MD5_LOG");
                createString.append("(id INTEGER PRIMARY KEY, ");
                createString.append("MD5 TEXT NOT NULL, ");
                createString.append("size INTEGER NOT NULL, ");
                createString.append("chunk_size INTEGER NOT NULL, ");

                for(int i = 1; i <= 7; i++)
                    createString.append(QString("Id_piece%1 TEXT NULL, ")
                                        .arg(i));

                createString.append("counter INTEGER NOT NULL)");

                QSqlQuery query;
                if (!query.exec(createString))
                {
                    qDebug() <<  "Error = " << query.lastError();
                }
                else
                {
                    for(int i = 0; i < model->rowCount(); i++)
                    {
                        QString insertString = QString("INSERT INTO MD5_LOG");
                        insertString.append("(id, ");
                        insertString.append("MD5, ");
                        insertString.append("size, ");
                        insertString.append("chunk_size, ");
                        for(int j = 1; j <= 7; j++)
                            insertString.append(QString("Id_piece%1, ")
                                                .arg(j));
                        insertString.append("counter)");

                        insertString.append("VALUES(:id, ");
                        insertString.append(":MD5, ");
                        insertString.append(":size, ");
                        insertString.append(":chunk_size, ");
                        for(int j = 1; j <= 7; j++)
                            insertString.append(QString(":Id_piece%1, ")
                                                .arg(j));
                        insertString.append(":counter)");

                        QSqlQuery query;
                        query.prepare(insertString);
                        query.bindValue(":MD5", model->record(i).value(1).toString());
                        query.bindValue(":size", model->record(i).value(2).toInt());
                        query.bindValue(":chunk_size", model->record(i).value(3).toInt());
                        query.bindValue(":Id_piece1", model->record(i).value(4).toString());
                        query.bindValue(":Id_piece2", model->record(i).value(5).toString());
                        query.bindValue(":Id_piece3", model->record(i).value(6).toString());
                        query.bindValue(":Id_piece4", model->record(i).value(7).toString());
                        query.bindValue(":Id_piece5", model->record(i).value(8).toString());
                        query.bindValue(":Id_piece6", model->record(i).value(9).toString());
                        query.bindValue(":Id_piece7", model->record(i).value(10).toString());
                        query.bindValue(":counter", model->record(i).value(11).toInt());

                        if (!query.exec())
                        {
                            qDebug() <<  "Error = " << query.lastError();
                        }

                        query.finish();
                    }
                }
            }


            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }


    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

// Reorganize the database MD5_LOG.
void DatabaseControllerEngines::reorganizeDatabase()
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("MD5log.db");

        if(db.open())
        {
            QString vacuumString = QString("VACUUM");

            QSqlQuery query;
            if (!query.exec(vacuumString))
            {
                qDebug() <<  "Error = " << query.lastError();
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}
