#include "playlistview.h"
#include <QStringList>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QString>
//#include "dboperate.h"

#include "tagmanager.h"

#include "LAVA/lava.h"

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

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(rowDoubleClicked(QModelIndex)));

}

void PlayListView::setUpModel(const int listIndex)
{
//    QString listName = getListName(listIndex);
    QString listName = "list_0";
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
    this->setColumnHidden(model->fieldIndex("path"), true);

}

void PlayListView::refreshModel()
{
    model->select();
}

void PlayListView::rowDoubleClicked(const QModelIndex &index)
{
    QModelIndex i = model->index(index.row(), model->fieldIndex("path"), QModelIndex());
    qDebug() << i;
    QString path = model->data(i, 0).toString();
    qDebug() << path;
    LAVA::Core::instance()->load_file(path.toStdString().c_str());
}
