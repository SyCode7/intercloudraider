#include "databasecontrollermetadata.h"

DatabaseControllerMetadata::DatabaseControllerMetadata()
{
}

DatabaseControllerMetadata::~DatabaseControllerMetadata()
{
}

// Create the table metadata and store the file metadata.
QList<QVariant> DatabaseControllerMetadata::insertEntries(QString title, qint64 size,
                                                          QString format, QString location)
{
    QList<QVariant> result;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("metadata.db");

        if(db.open())
        {
            QString createString = "CREATE TABLE IF NOT EXISTS metadata";
            createString.append("(file_id INTEGER PRIMARY KEY, ");
            createString.append("Title TEXT NOT NULL, ");
            createString.append("Size INTEGER NOT NULL, ");
            createString.append("Format TEXT NOT NULL, ");
            createString.append("Location TEXT NOT NULL)");

            QSqlQuery query;
            if (query.exec(createString))
            {
                QString insertString = "INSERT INTO metadata";
                insertString.append("(file_id, ");
                insertString.append("Title, ");
                insertString.append("Size, ");
                insertString.append("Format, ");
                insertString.append("Location)");

                insertString.append("VALUES(:file_id, ");
                insertString.append(":Title, ");
                insertString.append(":Size, ");
                insertString.append(":Format, ");
                insertString.append(":Location)");

                query.prepare(insertString);
                query.bindValue(":Title", title);
                query.bindValue(":Size", size);
                query.bindValue(":Format", format);
                query.bindValue(":Location", location);

                if (!query.exec())
                {
                    qDebug() <<  "Error = " << query.lastError();

                    result.append(false);
                }
                else
                {
                    result.append(true);
                    result.append(query.lastInsertId().toInt());
                }
            }
            else
            {
                qDebug() <<  "Error = " << query.lastError();

                result.append(false);
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();

            result.append(false);
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return result;
}

QString DatabaseControllerMetadata::getTitle(int file_id)
{
    QString title;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("metadata.db");

        if(db.open())
        {
            QString selectString = QString("SELECT Title FROM metadata WHERE file_id = %1")
                    .arg(file_id);

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);

            title = model->record(0).value(0).toString();

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            title = getTitle(file_id);
        }

    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return title;
}

// Remove a list of entries from the table metadata.
void DatabaseControllerMetadata::deleteEntries(QList<int> file_idList)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("metadata.db");

        if(db.open())
        {
            QString deleteString = QString("DELETE FROM metadata WHERE file_id BETWEEN %1 AND %2")
                    .arg(file_idList.last() - file_idList.size() + 1)
                    .arg(file_idList.last());

            QSqlQuery query;
            if (!query.exec(deleteString))
            {
                qDebug() <<  "Error = " << query.lastError();
            }

            db.close();
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}

// Retrieve metadata from the table metadata.
QList<int> DatabaseControllerMetadata::returnMetaInfo(int file_id)
{
    QList<int> parameters;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("metadata.db");

        if(db.open())
        {
            QString selectString =
            QString("SELECT Size FROM metadata WHERE file_id = %1")
                    .arg(file_id);

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);

            parameters.append(model->record(0).value(0).toInt());

            QString tablename = "";
            tablename.append(QString::number(file_id));
            tablename.prepend("[");
            tablename.append("]");
            tablename.replace(" ", "_");

            QString selectString2 = QString("SELECT * FROM %1").arg(tablename);

            QSqlQueryModel *model2 = new QSqlQueryModel();
            model2->setQuery(selectString2);
            while (model2->canFetchMore())
                model2->fetchMore();

            parameters.append(model2->rowCount());

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            parameters = returnMetaInfo(file_id);
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return parameters;
}

// Remove one entry from the table metadata.
void DatabaseControllerMetadata::deleteEntry(int file_id)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("metadata.db");

        if(db.open())
        {
            QString deleteString = QString("DELETE FROM metadata WHERE file_id = %1")
                    .arg(file_id);

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

// Create table file_id (file descriptor) including md5, chunk_size, offset.
void DatabaseControllerMetadata::createDescription(QString tablename)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("metadata.db");

        // Table names that are valid without using brackets around them should be
        // any alphanumeric combination that doesn't start with a digit:
        // abc123 - valid
        // 123abc - not valid
        // abc_123 - valid
        // _123abc - valid
        // abc-abc - not valid (looks like an expression)
        // abc.abc - not valid (looks like a database.table notation)

        tablename.prepend("[");
        tablename.append("]");
        tablename.replace(" ", "_");

        if(db.open())
        {
            QString createString = QString("CREATE TABLE IF NOT EXISTS %1")
                    .arg(tablename);
            createString.append("(file_id INTEGER PRIMARY KEY, ");
            createString.append("md5 TEXT NULL, ");
            createString.append("offset INTEGER NULL, ");
            createString.append("chunk_size INTEGER NULL)");

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

void DatabaseControllerMetadata::insertDescription(QString tablename, int i, QString md5,
                                                   int offset, int chunk_size)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("metadata.db");

        // Table names that are valid without using brackets around them should be
        // any alphanumeric combination that doesn't start with a digit:
        // abc123 - valid
        // 123abc - not valid
        // abc_123 - valid
        // _123abc - valid
        // abc-abc - not valid (looks like an expression)
        // abc.abc - not valid (looks like a database.table notation)

        tablename.prepend("[");
        tablename.append("]");
        tablename.replace(" ", "_");

        if(db.open())
        {
            QString insertString = QString("INSERT INTO %1").arg(tablename);
            insertString.append("(file_id, ");
            insertString.append("md5, ");
            insertString.append("offset, ");
            insertString.append("chunk_size)");

            insertString.append("VALUES(:file_id, ");
            insertString.append(":md5, ");
            insertString.append(":offset, ");
            insertString.append(":chunk_size)");

            QSqlQuery query;
            query.prepare(insertString);
            query.bindValue(":file_id", i);
            query.bindValue(":md5", md5);
            query.bindValue(":offset", offset);
            query.bindValue(":chunk_size", chunk_size);

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

void DatabaseControllerMetadata::insertDescription(QString tablename,
                                                   QMap<int, QVariantList> Description)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("metadata.db");

        // Table names that are valid without using brackets around them should be
        // any alphanumeric combination that doesn't start with a digit:
        // abc123 - valid
        // 123abc - not valid
        // abc_123 - valid
        // _123abc - valid
        // abc-abc - not valid (looks like an expression)
        // abc.abc - not valid (looks like a database.table notation)

        tablename.prepend("[");
        tablename.append("]");
        tablename.replace(" ", "_");

        if(db.open())
        {
            QMap<int, QVariantList>::const_iterator j = Description.constBegin();
            while(j != Description.constEnd())
            {
                QVariantList entry = j.value();

                QString insertString = QString("INSERT INTO %1").arg(tablename);
                insertString.append("(file_id, ");
                insertString.append("md5, ");
                insertString.append("offset, ");
                insertString.append("chunk_size)");

                insertString.append("VALUES(:file_id, ");
                insertString.append(":md5, ");
                insertString.append(":offset, ");
                insertString.append(":chunk_size)");

                QSqlQuery query;
                query.prepare(insertString);
                query.bindValue(":file_id", j.key());
                query.bindValue(":md5", entry[0].toString());
                query.bindValue(":offset", entry[1].toInt());
                query.bindValue(":chunk_size", entry[2].toInt());

                if (!query.exec())
                {
                    qDebug() <<  "Error = " << query.lastError();
                }

                query.finish();

                j++;
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

// Retrieve file description (md5, offset, chunk_size) from the table file_id.
QVariantList *DatabaseControllerMetadata::returnDescription(int file_id)
{
    QVariantList* Description;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("metadata.db");

        QString tablename = "";
        tablename.append(QString::number(file_id));
        tablename.prepend("[");
        tablename.append("]");
        tablename.replace(" ", "_");

        if(db.open())
        {
            QString selectString = QString("SELECT * FROM %1").arg(tablename);

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);
            while (model->canFetchMore())
                model->fetchMore();

            Description = new QVariantList[model->rowCount()];

            for(int i = 0; i < model->rowCount(); i++)
            {
                QVariantList entry;

                // Retrieve md5, offset, chunk_size.
                entry.append(model->record(i).value(1).toString());
                entry.append(model->record(i).value(2).toInt());
                entry.append(model->record(i).value(3).toInt());

                Description[i] = entry;
            }

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            Description = returnDescription(file_id);
        }
    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return Description;
}

int DatabaseControllerMetadata::getNoChunks(int file_id)
{
    int no_chunks;

    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("metadata.db");

        QString tablename = "";
        tablename.append(QString::number(file_id));
        tablename.prepend("[");
        tablename.append("]");
        tablename.replace(" ", "_");

        if(db.open())
        {
            QString selectString = QString("SELECT * FROM %1").arg(tablename);

            QSqlQueryModel *model = new QSqlQueryModel();
            model->setQuery(selectString);
            while (model->canFetchMore())
                model->fetchMore();

            no_chunks = model->rowCount();

            db.close();
        }
        else
        {
            qDebug() << "Error = " << db.lastError();
            no_chunks = getNoChunks(file_id);
        }

    }

    QSqlDatabase::removeDatabase("qt_sql_default_connection");

    return no_chunks;
}

// Delete the table [file_id].
void DatabaseControllerMetadata::deleteTable(int file_id)
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("metadata.db");

        QString tablename = "";
        tablename.append(QString::number(file_id));
        tablename.prepend("[");
        tablename.append("]");
        tablename.replace(" ", "_");

        if(db.open())
        {
            QString dropString = QString("DROP TABLE IF EXISTS %1")
                    .arg(tablename);

            QSqlQuery query;
            if (!query.exec(dropString))
            {
                qDebug() <<  "Error = " << query.lastError();
            }
            else
            {
                qDebug() << QString("The table %1 is removed").arg(tablename);
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

// Reorganize the database metadata.
void DatabaseControllerMetadata::reorganizeDatabase()
{
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("metadata.db");

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
