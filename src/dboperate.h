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
        );

QString getListName(const int listIndex);

void deleteSong(const int listIndex, const int id);

void setPlaying(const int listIndex, const int id);

int getPlayingSong(const int listIndex);

#endif // DBOPERATE_H
