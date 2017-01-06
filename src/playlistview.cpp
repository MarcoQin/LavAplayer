#include "playlistview.h"
#include <QStringList>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include "dboperate.h"

#include "tagmanager.h"

#include "playstatedelegate.h"

PlayListView::PlayListView(QWidget *parent) :
    QTableView(parent)
{
    state = new P_State;
    state->state = Stopped;
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setAlternatingRowColors(true);
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->setVisible(false);

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

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(rowDoubleClicked(QModelIndex)));

}

void PlayListView::setUpModel(const int listIndex)
{
    m_listIndex = listIndex;
    QString listName = getListName(listIndex);
//    QString listName = "list_0";
    model = new QSqlRelationalTableModel(this);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable(listName);


    model->setHeaderData(model->fieldIndex("title"), Qt::Horizontal, tr("Title"));
    model->setHeaderData(model->fieldIndex("artist"), Qt::Horizontal, tr("Artist"));
    model->setHeaderData(model->fieldIndex("album"), Qt::Horizontal, tr("Album"));
    model->setHeaderData(model->fieldIndex("track"), Qt::Horizontal, tr("#"));
    model->setHeaderData(model->fieldIndex("id"), Qt::Horizontal, tr(""));

    horizontalHeader()->moveSection(model->fieldIndex("track"), 1);

    if (!model->select()) {
        QMessageBox::critical(this, "Unable to initialize Database",
                    "Error initializing database: " + model->lastError().text());
        return;
    }

    this->setModel(model);
    while(model->canFetchMore()) {
        model->fetchMore();
    }

    QList<QString> fieldToHide;
    fieldToHide << "path" << "bitrate" << "samplerate" << "length" << "genre" << "year" << "is_playing";

    for (auto i = fieldToHide.begin(); i != fieldToHide.end(); ++i) {
        this->setColumnHidden(model->fieldIndex(*i), true);
    }

    horizontalHeader()->moveSection(model->fieldIndex("track"), 1);
    horizontalHeader()->moveSection(model->fieldIndex("id"), 0);
    setColumnWidth(model->fieldIndex("track"), 20);
    setColumnWidth(model->fieldIndex("title"), 300);
    setColumnWidth(model->fieldIndex("artist"), 200);



    setItemDelegateForColumn(0, new PlayStateDelegate(state));

}

void PlayListView::onSongStartPlay(int id)
{
    state->id = id;
    state->state = Playing;
    this->viewport()->repaint();
}

void PlayListView::onSongStartPlay()
{
    state->state = Playing;
    this->viewport()->repaint();
}

void PlayListView::onSongPaused()
{
    state->state = Paused;
    this->viewport()->repaint();
}

void PlayListView::onSongStopped()
{
    state->state = Stopped;
    this->viewport()->repaint();
}

void PlayListView::refreshModel()
{
    model->select();
}

void PlayListView::rowDoubleClicked(const QModelIndex &index)
{
    m_rowIndex = index.row();
    const QSqlRecord rowInfo = model->record(m_rowIndex);
    emit onSongDoubleClicked(rowInfo);
}

void PlayListView::playNext()
{
    playNext(false);
}

void PlayListView::playPrevious()
{
    playNext(true);
}

void PlayListView::playNext(bool reverse)
{
    qDebug() << model->rowCount();
    if (reverse) {
        m_rowIndex = m_rowIndex > 0 ? --m_rowIndex : model->rowCount() - 1;
    } else {
        m_rowIndex = m_rowIndex >= model->rowCount() - 1 ? 0 : ++m_rowIndex;
    }
    playAtRowIndex(m_rowIndex);
}

void PlayListView::playAtRowIndex(int rowIndex)
{
    const QSqlRecord rowInfo = model->record(rowIndex);
    emit onSongDoubleClicked(rowInfo);
    QModelIndex index= model->index(rowIndex, 0,QModelIndex());
    this->scrollTo(index);
}

void PlayListView::onSongAboutToStop()
{
    playNext();
}

void PlayListView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        QItemSelectionModel *select = selectionModel();
        if (select->hasSelection()) {
            QModelIndexList indexs = select->selectedRows();
            QList<int> ids;
            for (auto i = indexs.begin(); i != indexs.end(); ++i) {
                QSqlRecord rowInfo = model->record(i->row());
                qDebug() << "delete: " << rowInfo.value("path").toString();
                ids << rowInfo.value("id").toInt();
            }
            for (auto i = ids.begin(); i != ids.end(); ++i) {
                qDebug() << *i;
                deleteSong(m_listIndex, *i);
            }
            model->select();
        }
    }
}

void PlayListView::tryStartPlay()
{
    int id = getPlayingSong(m_listIndex);
    qDebug() << id;
    if (id != 0) {
        int rowIndex = 0;
        for (rowIndex; rowIndex < model->rowCount(); ++rowIndex) {
            const QSqlRecord rowInfo = model->record(rowIndex);
            if (rowInfo.value("id").toInt() == id) {
                playAtRowIndex(rowIndex);
                return;
            }
        }
    }
    playAtRowIndex(0);
}
