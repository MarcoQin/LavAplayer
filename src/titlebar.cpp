#include "titlebar.h"

using namespace UI;

TitleBar::TitleBar(QWidget *parent) :
    FloatWidget(parent)
{
    setRadius(0);
    setBackgroundColor(QColor(27, 27, 27, 100));
    setPosition(TopLeft);
    setCustomPositionOffset(QPoint(0, -barHight));

    closeBtn = new QPushButton(this);
    closeBtn->setStyleSheet("QPushButton{border-image:url(:/Picture/res/close-btn.png);}"
                         "QPushButton:hover{border-image:url(:/Picture/res/close-btn-1.png)}");
    closeBtn->setGeometry(8, 4, 12, 12);

    minBtn = new QPushButton(this);
    minBtn->setStyleSheet("QPushButton{border-image:url(:/Picture/res/min-btn.png);}"
                       "QPushButton:hover{border-image:url(:/Picture/res/min-btn-1.png)}");
    minBtn->setGeometry(28, 4, 12, 12);

    maxBtn = new QPushButton(this);
    maxBtn->setStyleSheet("QPushButton{border-image:url(:/Picture/res/max-btn.png);}"
                       "QPushButton:hover{border-image:url(:/Picture/res/max-btn-1.png)}");
    maxBtn->setGeometry(48, 4, 12, 12);


    connect(closeBtn, SIGNAL(clicked(bool)), this, SIGNAL(closeClicked()));
    connect(minBtn, SIGNAL(clicked(bool)), this, SIGNAL(minClicked()));
    connect(maxBtn, SIGNAL(clicked(bool)), this, SIGNAL(maxClicked()));
    resetSize(parent);
}


void TitleBar::resetSize(QWidget *parent)
{
    resize(parent->width(), barHight);
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
    }
    QMouseEvent* eventCopy = new QMouseEvent(*event);
    QApplication::instance()->postEvent(parent(), eventCopy);
}

void TitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    QMouseEvent* eventCopy = new QMouseEvent(*event);
    QApplication::instance()->postEvent(parent(), eventCopy);
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        static_cast<QWidget *>(parent())->move(event->globalPos() - dragPosition - QPoint(0, -barHight));
    }
    QMouseEvent* eventCopy = new QMouseEvent(*event);
    QApplication::instance()->postEvent(parent(), eventCopy);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit requetMaxmiumWindow();
    event->accept();
}
