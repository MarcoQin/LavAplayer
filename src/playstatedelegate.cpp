#include "playstatedelegate.h"
#include <QtWidgets>
#include "playlistview.h"

PlayStateDelegate::PlayStateDelegate(P_State *state)
{
    m_state = state;
}


void PlayStateDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    const QSqlRecord rowInfo = ((QSqlRelationalTableModel *)index.model())->record(index.row());
    if (index.row() % 2) {
        painter->fillRect(option.rect, option.palette.alternateBase());
    } else {
        painter->fillRect(option.rect, option.palette.base());
    }
    if (option.state & QStyle::State_Selected) {
        painter->setPen(QColor(200, 200, 200));
    } else {
        painter->setPen(QColor(90, 90, 90));
    }
    painter->drawText(option.rect.x() + 30, option.rect.y() + 20, QString::number(index.row() + 1));
    painter->setBrush(QColor(200, 200, 200));
    if (index.data().toInt() == m_state->id) {
        switch(m_state->state) {
        case Playing:
        {
            QPolygonF polygon;
            polygon << QPointF(option.rect.x() + 5, option.rect.y() + 10)
                    << QPointF(option.rect.x() + 5,  option.rect.y() + option.rect.height() - 10)
                    << QPointF(option.rect.x() + 16, option.rect.y() + option.rect.height()/2)
                    << QPointF(option.rect.x() + 5, option.rect.y() + 10);
            painter->drawPolygon(polygon, Qt::WindingFill);
            break;
        }
        case Paused:
        {
            QRect rect (option.rect.x() + 5, option.rect.y() + 10, 2, 10);
            painter->fillRect(rect, QColor(200, 200, 200));
            painter->translate(QPoint(4, 0));
            painter->fillRect(rect, QColor(200, 200, 200));
            painter->translate(QPoint(-4, 0));
            break;
        }
        default:
            break;

        }
    }
}
