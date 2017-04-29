#include "player.h"
#include "bullet.h"
#include <QDebug>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QMediaPlayer>

void Player::keyPressEvent(QKeyEvent *event)
{
    if (event->key()==Qt::Key_Left)
    {
        if (x()>=10) setPos(x()-10, y());
    }
    else if (event->key()==Qt::Key_Right)
    {
        ;
        if (x()+width+10 <= scene()->width()) setPos(x()+10, y());
    }
    else if (event->key()==Qt::Key_Up)
    {
        setPos(x(), y()-10);
    }
    else if (event->key()==Qt::Key_Down)
    {
        setPos(x(), y()+10);
    }
    else if (event->key()==Qt::Key_Space)
    {
        Bullet *bullet = new Bullet(this);
        scene()->addItem(bullet);
        bullet->setPos(x()+45, y()-50);
        if (bulletSound->state() == QMediaPlayer::PlayingState)
        {
            bulletSound->setPosition(0);
        } else bulletSound->play();
    }
}


bool Player::init()
{
    height = 100;
    width = 100;
    bulletSound = new QMediaPlayer;
    bulletSound->setMedia(QUrl("qrc:/sounds/M1 Garand Single-SoundBible.com-1941178963.mp3"));

    //    player->setRect(0,0,100,100);
    setPixmap(QPixmap(":/images/player2.gif"));

    return true;
}
