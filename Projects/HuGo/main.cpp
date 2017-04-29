// URL https://www.youtube.com/watch?v=eY5NQLHTeys
// http://www191.lunapic.com/editor/

#include "mainwindow.h"
#include <QApplication>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QObject>




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.init();

    return a.exec();
}



/*
#include "game.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Game* game = new Game;

    game->init();
    game->show();

    return a.exec();
}
*/
