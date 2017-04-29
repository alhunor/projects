#include "enemy.h"
#include <stdlib.h>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

#include <QObject>

//const int Enemy::width = 100;
//const int Enemy::height = 100;

Enemy::Enemy(int ex, QGraphicsItem* parent) : QGraphicsPixmapItem(parent)
{
    //setRect(0,0,Enemies::width,Enemies::height);
    if (rand() % 2 ==0)
    {
        setPixmap(QPixmap(":/images/asteroid23.png"));
    } else
    {
        setPixmap(QPixmap(":/images/meteorite.png"));
    }
    setTransformOriginPoint(50,50);
    rot = 0;

    setPos(ex, 0);
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(move()));
    timer->start(250);
}

Enemy::~Enemy()
{
    timer->stop();
    disconnect(timer,SIGNAL(timeout()),this,SLOT(move()));
    delete timer;
}


void Enemy::move()
{
    setRotation(rot);
    rot+=10;

    setPos(x(), y()+10);
    if (pos().y()>scene()->height())
    {
        scene()->removeItem(this);
        delete this;
    }
}

