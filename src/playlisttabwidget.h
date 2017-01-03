#ifndef PLAYLISTTABWIDGET_H
#define PLAYLISTTABWIDGET_H

#include <QTabWidget>

class PlayListView;

class PlayListTabWidget : public QTabWidget
{
    Q_OBJECT
public:
    PlayListTabWidget(QWidget *parent = nullptr);

public slots:
    PlayListView *createTab(bool makeCurrent = true);
};

#endif // PLAYLISTTABWIDGET_H
