#include "tagmanager.h"

#include <taglib/tbytevector.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2header.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/fileref.h>
#include <taglib/tstring.h>
#include <iostream>
#include <iomanip>

#include <string>
#include <QString>

using namespace std;

TagManager *TagManager::_instance = nullptr;

TagManager *TagManager::instance()
{
    return _instance ? _instance : new TagManager();
}

TagManager::TagManager(QObject *parent) : QObject(parent)
{

}

QString convert(TagLib::String s) {
    return QString::fromWCharArray(s.toCWString(), s.size());
}

QPixmap TagManager::getCover(const char *filename)
{
    TagLib::MPEG::File file(filename);
    TagLib::ID3v2::Tag *m_tag = file.ID3v2Tag(true);
    TagLib::ID3v2::FrameList frameList = m_tag->frameList("APIC");
    if (frameList.isEmpty()) {
        QPixmap cover(":/pic/images/default.png");
        emit gotCover(cover);
        return cover;
    } else {
        TagLib::ID3v2::AttachedPictureFrame *coverImg = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(frameList.front());
        QPixmap cover;
        cover.loadFromData((const uchar *) coverImg->picture().data(), coverImg->picture().size());
        emit gotCover(cover);
        return cover;
    }
}

TagInfo TagManager::getTagInfo(const char *filename)
{
    TagLib::FileRef f(filename);
    TagInfo info;
    info.path = filename;
    if (!f.isNull() && f.tag()) {
        TagLib::Tag *tag = f.tag();
        cout << "-- TAG (basic) --" << endl;
          cout << "title   - \"" << convert(tag->title()).toStdString()   << "\"" << endl;
          cout << "artist  - \"" << convert(tag->artist()).toStdString()  << "\"" << endl;
          cout << "album   - \"" << convert(tag->album()).toStdString()   << "\"" << endl;
          cout << "year    - \"" << tag->year()    << "\"" << endl;
          cout << "comment - \"" << convert(tag->comment()).toStdString() << "\"" << endl;
          cout << "track   - \"" << tag->track()   << "\"" << endl;
          cout << "genre   - \"" << tag->genre()  << "\"" << endl;
          info.title = convert(tag->title());
          if (info.title.isEmpty()) {
              std::string p(filename);
              auto start = p.rfind("/");
              auto stop = p.rfind(".");
              info.title = p.substr(start + 1, stop).c_str();
          }
          info.artist = convert(tag->artist());
          if (info.artist.isEmpty()) {
              info.artist = "N/A";
          }
          info.album = convert(tag->album());
          if (info.album.isEmpty()) {
              info.album = "N/A";
          }
          info.year = tag->year();
          info.track = tag->track();
          info.genre = convert(tag->genre());
    }


    if (!f.isNull() && f.audioProperties()) {
        TagLib::AudioProperties *properties = f.audioProperties();

        int seconds = properties->length() % 60;
        int minutes = (properties->length() - seconds) / 60;

        cout << "-- AUDIO --" << endl;
        cout << "bitrate     - " << properties->bitrate() << endl;
        cout << "sample rate - " << properties->sampleRate() << endl;
        cout << "channels    - " << properties->channels() << endl;
        cout << ("length      - " + QString("haha") + QString::number(12)).toStdString() << minutes << ":" << setfill('0') << setw(2) << seconds << endl;
        info.bitrate = properties->bitrate();
        info.samplerate = properties->sampleRate();
        info.length = properties->length();
    }

    return info;
}
