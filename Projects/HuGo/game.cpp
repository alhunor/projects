#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMediaPlayer>

#include <QBrush>
#include <QImage>

#include "game.h"
#include "bullet.h"
#include "enemy.h"
#include "player.h"

Game::Game() : player(NULL), scene(NULL), score(NULL)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(800,600);
}

Game::~Game()
{
    delete player;
    delete scene;
    delete score;
}


bool Game::init()
{
    // Create scene and point the view to the scene
    scene = new QGraphicsScene();
    scene->setSceneRect(0,0, 800,600);
    setBackgroundBrush(QBrush(QImage(":/images/space-background.jpg")));
    setScene(scene);

    // Create player
    player = new Player(this, NULL);

    // add player to scene
    scene->addItem(player);
    // make it focusable in order to receive events
    player->setFlag(QGraphicsItem::ItemIsFocusable);
    player->setFocus();
    player->setPos((width()-player->width) / 2, height()-player->height);

    enemies = new Enemies(this);
    scene->addItem(enemies); // to receive timer event

    // score
    score =  new Score();
    scene->addItem(score);


    // background music

    QMediaPlayer * musicplayer = new QMediaPlayer;
    musicplayer->setMedia(QUrl("qrc:/sounds/Midnight-Crawlers_Looping.mp3"));
    musicplayer->play();


    return true;
} // bool Game::init()



Enemies::Enemies(Game* g) : game(g)
{
    QTimer* etimer =  new QTimer();
    connect(etimer, SIGNAL(timeout()), this, SLOT(spawn()));
    etimer->start(2000);
}

void Enemies::spawn()
{
    int w = game->scene->width() - Enemy::width;
    Enemy *enemy = new Enemy (rand() % w);
    game->scene->addItem(enemy);
}
