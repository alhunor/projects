#ifndef BULLET_H
#define BULLET_H

//#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include "player.h"


class Bullet : public QObject, public QGraphicsPixmapItem //QGraphicsRectItem
{
    Q_OBJECT
public:
    Bullet(Player* p);
    ~Bullet();
    int height;
private:
    QTimer* timer;
    Player* player;

public slots:
    void move();
};

#endif // BULLET_H
