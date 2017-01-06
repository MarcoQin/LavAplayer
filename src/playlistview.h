#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include <QtWidgets>
#include <QtSql>
#include <QList>
#include <QUrl>

enum PState {
    Playing = 0,
    Paused,
    Stopped
};

struct P_State {
    int id;
    bool is_playing;
    PState state;
};

class PlayListView : public QTableView
{
    Q_OBJECT
public:
    PlayListView(QWidget *parent);
    void setUpModel(const int listIndex);
signals:
    void onSongDoubleClicked(const QSqlRecord &rowInfo);
public slots:
    void onSongStartPlay(int id);
    void onSongStartPlay();
    void onSongPaused();
    void onSongStopped();
    void refreshModel();
    void rowDoubleClicked(const QModelIndex &index);
    void keyPressEvent(QKeyEvent *event);
    void onSongAboutToStop();
    void playNext(bool reverse);
    void playNext();
    void playPrevious();
    void tryStartPlay();
    void scrollToPlayingSong();
private:
    void playAtRowIndex(int rowIndex);
    int m_rowIndex;
    QSqlRelationalTableModel *model;
    int m_listIndex;
    P_State *state;
};

#endif // PLAYLISTVIEW_H
