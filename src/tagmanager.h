#ifndef TAGMANAGER_H
#define TAGMANAGER_H

#include <QObject>
#include <QPixmap>
struct TagInfo {
    QString path;
    QString title;
    QString artist;
    QString album;
    int year;
    int track;
    QString genre;
    int bitrate;
    int samplerate;
    int length;
};

class TagManager : public QObject
{
    Q_OBJECT
public:
    static TagManager *instance();
    QPixmap getCover(const char *filename);
    TagInfo getTagInfo(const char *filename);
private:
    explicit TagManager(QObject *parent = 0);
    static TagManager *_instance;

signals:
    void gotCover(QPixmap cover);

public slots:
};

#endif // TAGMANAGER_H
