#include "playlisttabwidget.h"
#include "playlistview.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

PlayListTabWidget::PlayListTabWidget(QWidget *parent) :
    QTabWidget(parent)
{
    QTabBar *tabBar = this->tabBar();
    tabBar->setTabsClosable(true);
    tabBar->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
    tabBar->setMovable(true);
    tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
    setTabPosition(QTabWidget::South);

    QFile f(":/qss/res/tabwidget.qss");
    if (!f.exists()) {
        qDebug() << "tabwidget.qss is not exists!!";
    } else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        QString s = ts.readAll();
        setStyleSheet(s);
        f.close();
    }

    setGeometry(19, 229, 881, 281);
    setAcceptDrops(true);
}


PlayListView *PlayListTabWidget::createTab(bool makeCurrent)
{
    PlayListView *view = new PlayListView(this);
    int listIndex = addTab(view, "新建播放列表");
    view->setUpModel(listIndex);
    if (makeCurrent) {
        setCurrentWidget(view);
    }
    return view;
}
