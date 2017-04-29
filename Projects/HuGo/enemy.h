#ifndef ENEMY_H
#define ENEMY_H

#include <QGraphicsPixmapItem>
//#include <QGraphicsRectItem>
#include <QObject>
#include <QTimer>
#include "game.h"


class Enemy : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Enemy(int ex, QGraphicsItem* parent =0);
    ~Enemy();
    static const int width = 100;
    static const int height = 100;
    int rot;

private:
    QTimer* timer;

public slots:
    void move();
};


#endif // ENEMY_H
