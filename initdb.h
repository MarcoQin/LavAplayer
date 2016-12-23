#ifndef INITDB_H
#define INITDB_H

#include <QtSql>
#include <QString>

QSqlError initDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("playlist");

    if (!db.open())
        return db.lastError();

    QStringList tables = db.tables();
    if (!tables.isEmpty()) {
        return QSqlError();
    }

    QSqlQuery q;
    QString tableName = "list_" + QString::number(tables.length());
    QString sql = "create table " + tableName + " (id INTEGER PRIMARY KEY AUTOINCREMENT, path VARCHAR, title VARCHAR, artist VARCHAR, album VARCHAR, bitrate INTEGER DEFAULT 0, samplerate INTEGER DEFAULT 0, length INTEGER DEFAULT 0,  genre INTEGER, track VARCHAR, year INTEGER, is_playing INTEGER(2) DEFAULT 0)";

    if (!q.exec(sql))
        return q.lastError();

}

#endif // INITDB_H
