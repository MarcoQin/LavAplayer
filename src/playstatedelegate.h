#ifndef PLAYSTATEDELEGATE_H
#define PLAYSTATEDELEGATE_H
#include <QStyledItemDelegate>

struct P_State;

class PlayStateDelegate : public QStyledItemDelegate
{
public:
    PlayStateDelegate(P_State *state);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const Q_DECL_OVERRIDE;
//    QSize sizeHint(const QStyleOptionViewItem &option,
//                   const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    P_State *m_state;
};

#endif // PLAYSTATEDELEGATE_H
