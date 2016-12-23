#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QtWidgets>
#include <QUrl>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
    void OnFilesDrop(QList<QUrl>);
protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
