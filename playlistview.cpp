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

    QList<QString> fieldToHide;
    fieldToHide << "path" << "bitrate" << "samplerate" << "length" << "genre" << "year" << "is_playing";

    for (auto i = fieldToHide.begin(); i != fieldToHide.end(); ++i) {
        this->setColumnHidden(model->fieldIndex(*i), true);
    }

    horizontalHeader()->moveSection(model->fieldIndex("track"), model->fieldIndex("id") + 1);
    setColumnWidth(model->fieldIndex("track"), 20);
    setColumnWidth(model->fieldIndex("title"), 300);
    setColumnWidth(model->fieldIndex("artist"), 200);

}

void PlayListView::refreshModel()
{
    model->select();
}

void PlayListView::rowDoubleClicked(const QModelIndex &index)
{
    const QSqlRecord rowInfo = model->record(index.row());
    emit onSongDoubleClicked(rowInfo);
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
            refreshModel();
        }
    }
}
