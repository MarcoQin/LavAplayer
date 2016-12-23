#ifndef PLAYLISTVIEW_H
#define PLAYLISTVIEW_H

#include <QtWidgets>
#include <QtSql>
#include <QList>
#include <QUrl>

class PlayListView : public QTableView
{
    Q_OBJECT
public:
    PlayListView(QWidget *parent);
    void setUpModel(const int listIndex);
public slots:
    void insertSongs(QList<QUrl> urls);
private:
    QSqlRelationalTableModel *model;
};

#endif // PLAYLISTVIEW_H
