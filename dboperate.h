#ifndef DBOPERATE_H
#define DBOPERATE_H
#include <QtSql>
#include <QString>

void addSong(
        const int listIndex,
        const QString &path,
        const QString &title,
        const QString &artist,
        const QString &album,
        const int bitrate,
        const int samplerate,
        const int length,
        const QString genre,
        const int track,
        const int year
        )
{
    QSqlQuery q;
    q.prepare("insert into list_" + QString::number(listIndex) + " (path, title, artist, album, bitrate, samplerate, length, genre, track, year) values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue(path);
    q.addBindValue(title);
    q.addBindValue(artist);
    q.addBindValue(album);
    q.addBindValue(bitrate);
    q.addBindValue(samplerate);
    q.addBindValue(length);
    q.addBindValue(genre);
    q.addBindValue(track);
    q.addBindValue(year);
    q.exec();
}

QString getListName(const int listIndex)
{
    return "list_" + QString::number(listIndex);
}

#endif // DBOPERATE_H