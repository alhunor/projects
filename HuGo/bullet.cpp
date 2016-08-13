#include "bullet.h"
#include "enemy.h"
#include "game.h"
#include "player.h"

#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QList>
#include <QDebug>

Bullet::Bullet(Player* p): player(p) //, QGraphicsPixmapItem(p)
{
    //setRect(0,0,10,50);
    setPixmap(QPixmap(":/images/bullet.gif"));
    height = 100;

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(move()));
    timer->start(50);
}

Bullet::~Bullet()
{
    timer->stop();
    disconnect(timer,SIGNAL(timeout()),this,SLOT(move()));
    delete timer;
}


void Bullet::move()
{
    setPos(x(), y()-10);
    QList<QGraphicsItem*> collisions = collidingItems();
    int n = collisions.size();
    for (int i=0; i<n; ++i)
    {
        if (typeid(*(collisions[i])) == typeid(Enemy))
        {
            scene()->removeItem(collisions[i]);
            scene()->removeItem(this);
            player->game->score->increase();
            delete this;
            delete collisions[i];
            return;
        }
    }

    if (pos().y()+height<0)
    {
        scene()->removeItem(this);
        delete this;
    }
} // void Bullet::move()
