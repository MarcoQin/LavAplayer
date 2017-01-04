#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include <QtWidgets>
#include <QtSql>
#include <QList>
#include <QUrl>

struct P_State {
    int id;
    bool is_playing;
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
    void refreshModel();
    void rowDoubleClicked(const QModelIndex &index);
    void keyPressEvent(QKeyEvent *event);
private:
    QSqlRelationalTableModel *model;
    int m_listIndex;
    P_State *state;
};

#endif // PLAYLISTVIEW_H
