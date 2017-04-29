#ifndef GAME_H
#define GAME_H

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "enemy.h"
#include "score.h"
#include "player.h"

class Enemies;

class Game : public QGraphicsView
{
public:
    Game();
    ~Game();
    bool init();
    Score* score;
    QGraphicsScene* scene;

private:
    Player* player;
    Enemies* enemies;
};




class Enemies : public QObject, public QGraphicsRectItem
{
   Q_OBJECT
public:
    Enemies(Game* g);
    ~Enemies() {}
    Game* game;

public slots:
    void spawn();

};

#endif // GAME_H
