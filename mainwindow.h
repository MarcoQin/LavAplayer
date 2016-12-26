#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QtWidgets>
#include <QUrl>
#include <QtSql>

#include "titlebar.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void cbk(uint8_t *stream, int len);
signals:
    void onAddSong();
public slots:
    void onDoubleClickSong(const QSqlRecord &rowInfo);
    void setMinimumWindow();
    void showEvent(QShowEvent *event);
    void resetTitleBar();
    void moveEvent(QMoveEvent *event);
protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private:
    void connectSignals();
    Ui::MainWindow *ui;
    UI::TitleBar *titleBar;
};

#endif // MAINWINDOW_H
