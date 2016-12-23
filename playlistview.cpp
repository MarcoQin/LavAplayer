#include "playlistview.h"
#include <QStringList>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include "dboperate.h"

#include "tagmanager.h"

PlayListView::PlayListView(QWidget *parent) :
    QTableView(parent)
{
//    setRowCount(4);
//    setColumnCount(4);
//    setEditTriggers(QAbstractItemView::DoubleClicked
//                                | QAbstractItemView::SelectedClicked);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setAlternatingRowColors(true);
    horizontalHeader()->setStretchLastSection(true);
//    horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    verticalHeader()->setVisible(false);
    QStringList headerLabels;
    headerLabels << "Title" << "Artist" << "Album" << "Length";
//    setHorizontalHeaderLabels(headerLabels);

    QFile f(":/qss/res/tablewidget.qss");
    if (!f.exists()) {
        qDebug() << "tablewidget.qss is not exists!!";
    } else {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        QString s = ts.readAll();
        setStyleSheet(s);
        f.close();
    }

}

void PlayListView::setUpModel(const int listIndex)
{
    QString listName = getListName(listIndex);
//    QString listName = "list_0";
    model = new QSqlRelationalTableModel(this);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable(listName);

    model->setHeaderData(model->fieldIndex("title"), Qt::Horizontal, tr("Title"));

    if (!model->select()) {
        QMessageBox::critical(this, "Unable to initialize Database",
                    "Error initializing database: " + model->lastError().text());
        return;
    }

    this->setModel(model);

}

void PlayListView::insertSongs(QList<QUrl> urls)
{
    foreach (const QUrl &url, urls) {
        QString fileName = url.toLocalFile();
        qDebug() << "Dropped file:" << fileName;
        TagInfo info = TagManager::instance()->getTagInfo(fileName.toStdString().c_str());
        addSong(0, info.path, info.title, info.artist, info.album, info.bitrate, info.samplerate, info.length, info.genre, info.track, info.year);
    }
    model->select();
}
