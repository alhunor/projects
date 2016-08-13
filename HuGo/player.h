#ifndef MYRECT
#define MYRECT

#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QMediaPlayer>

//#include "game.h"
class Game;
class Player : public QGraphicsPixmapItem //QGraphicsRectItem
{
public:
    Player(Game* g, QGraphicsItem* parent) : game(g), bulletSound(NULL), QGraphicsPixmapItem(parent) { init(); }

    ~Player()
    {
        if (bulletSound) delete bulletSound;
    }

    bool init();
    void keyPressEvent(QKeyEvent* event);
    Game* game;
    int height, width;
private:
    QMediaPlayer * bulletSound;
};

#endif // MYRECT

