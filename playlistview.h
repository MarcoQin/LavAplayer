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
    void refreshModel();
    void rowDoubleClicked(const QModelIndex &index);
private:
    QSqlRelationalTableModel *model;
};

#endif // PLAYLISTVIEW_H
