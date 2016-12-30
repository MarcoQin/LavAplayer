#include "dboperate.h"


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

void deleteSong(const int listIndex, const int id)
{
    QSqlQuery q;
    q.prepare("delete from list_" + QString::number(listIndex) + " where id = ?;");
    q.addBindValue(id);
    q.exec();
}

void setPlaying(const int listIndex, const int id)
{
    QSqlQuery q;
    QString table_name = "list_" + QString::number(listIndex);
    q.prepare("update " + table_name + " set is_playing = 0 where is_playing = 1; ");
    q.exec();
    q.prepare("update " + table_name + " set is_playing = 1" + " where id = ?;");
    q.addBindValue(id);
    q.exec();
}

QString getListName(const int listIndex)
{
    return "list_" + QString::number(listIndex);
}
