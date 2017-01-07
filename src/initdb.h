#ifndef INITDB_H
#define INITDB_H

#include <QtSql>
#include <QString>
#include <QtCore>
#include <QDebug>
#include <QDir>

QSqlError initDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    QString homePath =  QDir::homePath();
    QString lavaPath = homePath + "/.LavAplayer";
    QDir dir(lavaPath);
    if (!dir.exists()) {
        QDir().mkdir(lavaPath);
    }
    QString dbPath = dir.filePath("playlist");

    qDebug() << dbPath;

    QFile f(dbPath);
    if (!f.exists()) {
        f.open(QIODevice::WriteOnly);
    }
    f.close();

    db.setDatabaseName(dbPath);

    if (!db.open()) {
        return db.lastError();
    }

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
